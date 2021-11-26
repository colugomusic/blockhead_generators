#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace delay {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
	, delay_{ float(SR()) }
	, SR_vec_(float(SR()))
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	static const ml::DSPVector EPSILON { 0.000001f };
	static const ml::DSPVector TWO { 2.0f };
	static const ml::DSPVectorArray<2> ONE_2 { 1.0f };

	AudioData data(plugin_, buffer);

	auto time = data.envelopes.time.search_vec(block_positions());
	auto feedback = data.envelopes.feedback.search_vec(block_positions());
	auto dampener_freq = data.envelopes.dampener.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	time = ml::pow(ml::clamp(time, EPSILON, SR_vec_), TWO);

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	//for (int row = 0; row < 2; row++)
	//{
	//	filters_[row].mCoeffs = ml::Lopass::coeffs(math::convert::linear_to_filter_hz(dampener_freq) / SR(), 1.0f);
	//}

	const auto dampener = [this, dampener_freq](const ml::DSPVectorArray<2>& x)
	{
		filter_(x, SR(), ml::repeatRows<2>(math::convert::linear_to_filter_hz(dampener_freq)));

		return filter_.lp();
	};

	out_vec = delay_(in_vec, ml::repeatRows<2>(time * SR_vec_ * 0.5f), ml::repeatRows<2>(feedback), dampener);

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	delay_ = snd::audio::FeedbackDelay<2> { float(SR()) };
	filter_ = snd::audio::filter::Filter_1Pole<2>{};
	SR_vec_ = ml::DSPVector(float(SR()));
}

} // delay
