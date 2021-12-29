#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace spring_reverb {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
	// set allpass filter coefficients
	mAp1.mGain = 0.75f;
	mAp2.mGain = 0.70f;
	mAp3.mGain = 0.625f;
	mAp4.mGain = 0.625f;
	mAp5.mGain = mAp6.mGain = 0.7f;
	mAp7.mGain = mAp8.mGain = 0.6f;
	mAp9.mGain = mAp10.mGain = 0.5f;

	// allocate delay memory
	mAp1.setMaxDelayInSamples(500.f);
	mAp2.setMaxDelayInSamples(500.f);
	mAp3.setMaxDelayInSamples(1000.f);
	mAp4.setMaxDelayInSamples(1000.f);
	mAp5.setMaxDelayInSamples(2600.f);
	mAp6.setMaxDelayInSamples(2600.f);
	mAp7.setMaxDelayInSamples(8000.f);
	mAp8.setMaxDelayInSamples(8000.f);
	mAp9.setMaxDelayInSamples(10000.f);
	mAp10.setMaxDelayInSamples(10000.f);
	mDelayL.setMaxDelayInSamples(3500.f);
	mDelayR.setMaxDelayInSamples(3500.f);
}

constexpr float kDecayLo = 0.01f, kDecayHi = 20.f;
ml::Projection unityToDecay(ml::projections::intervalMap({ 0, 1 }, { kDecayLo, kDecayHi }, ml::projections::log({ kDecayLo, kDecayHi })));

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	glide_size_.setGlideTimeInSamples(0.1f * SR());
	glide_decay_.setGlideTimeInSamples(0.1f * SR());

	const auto size = data.envelopes.size.search(block_positions());
	const auto decay = data.envelopes.decay.search(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	const float sr = float(SR());
	const float RT60const = 0.001f;

	float sizeU = ml::lerp(0.01f, 1.0f, size);
	float decayU = decay;

	// generate delay and feedback scalars
	float decayTime = unityToDecay(decayU);
	float decayIterations = decayTime / (sizeU * 0.5f);
	float feedback = (decayU < 1.0f) ? powf(RT60const, 1.0f / decayIterations) : 1.0f;

	if (buffer_count_++ < 1)
	{
		glide_size_.setValue(sizeU * 2.0f);
		glide_decay_.setValue(feedback);
	}

	// generate smoothed delay time and feedback gain vectors
	ml::DSPVector vSmoothDelay = glide_size_(sizeU * 2.0f);
	ml::DSPVector vSmoothFeedback = glide_decay_(feedback);

	// get the minimum possible delay in samples, which is the length of a DSPVector.
	ml::DSPVector vMin(kFloatsPerDSPVector);

	// get smoothed allpass times in samples
	ml::DSPVector delayParamInSamples = sr * vSmoothDelay;
	ml::DSPVector vt1 = max(0.00476f * delayParamInSamples, vMin);
	ml::DSPVector vt2 = max(0.00358f * delayParamInSamples, vMin);
	ml::DSPVector vt3 = max(0.00973f * delayParamInSamples, vMin);
	ml::DSPVector vt4 = max(0.00830f * delayParamInSamples, vMin);
	ml::DSPVector vt5 = max(0.029f * delayParamInSamples, vMin);
	ml::DSPVector vt6 = max(0.021f * delayParamInSamples, vMin);
	ml::DSPVector vt7 = max(0.078f * delayParamInSamples, vMin);
	ml::DSPVector vt8 = max(0.090f * delayParamInSamples, vMin);
	ml::DSPVector vt9 = max(0.111f * delayParamInSamples, vMin);
	ml::DSPVector vt10 = max(0.096f * delayParamInSamples, vMin);

	ml::DSPVectorArray<2> in_vec(in);

	// sum stereo inputs and diffuse with four allpass filters in series
	ml::DSPVector monoInput = (in_vec.constRow(0) + in_vec.constRow(1));
	ml::DSPVector diffusedInput = mAp4(mAp3(mAp2(mAp1(monoInput, vt1), vt2), vt3), vt4);

	// get delay times in samples, subtracting the constant delay of one DSPVector and clamping to zero
	ml::DSPVector vDelayTimeL = max(0.0313f * delayParamInSamples - vMin, ml::DSPVector(0.f));
	ml::DSPVector vDelayTimeR = max(0.0371f * delayParamInSamples - vMin, ml::DSPVector(0.f));

	// sum diffused input with feedback, and apply late diffusion of two more allpass filters to each channel
	ml::DSPVector vTapL = mAp7(mAp5(diffusedInput + mDelayL(mvFeedbackL, vDelayTimeL), vt5), vt7);
	ml::DSPVector vTapR = mAp8(mAp6(diffusedInput + mDelayR(mvFeedbackR, vDelayTimeR), vt6), vt8);

	// apply final allpass filter and gain, and store the feedback
	mvFeedbackR = mAp9(vTapL, vt9) * vSmoothFeedback;
	mvFeedbackL = mAp10(vTapR, vt10) * vSmoothFeedback;

	// append the left and right taps and return the stereo output
	const auto out_vec = ml::lerp(in_vec, ml::concatRows(vTapL, vTapR), ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	mAp1.clear();
	mAp2.clear();
	mAp3.clear();
	mAp4.clear();
	mAp5.clear();
	mAp6.clear();
	mAp7.clear();
	mAp8.clear();
	mAp9.clear();
	mAp10.clear();
	mDelayL.clear();
	mDelayR.clear();
	mvFeedbackL = ml::DSPVector();
	mvFeedbackR = ml::DSPVector();
}

} // spring_reverb