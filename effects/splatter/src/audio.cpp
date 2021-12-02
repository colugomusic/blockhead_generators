#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace splatter {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	const auto example = data.envelopes.example.search(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	ml::DSPVectorArray<2> filtered_input;

	for (int c = 0; c < 2; c++)
	{
		lp_[c].mCoeffs = ml::Lopass::coeffs(1000.0f / SR(), 1.0f);

		filtered_input.row(c) = lp_[c](in_vec.constRow(c));
	}

	follower_.set_SR(SR());
	follower_.set_attack(0.0001f);
	follower_.set_release(0.01f);

	const auto peaks = follower_(in_vec);
	const auto zero_crossings = zero_x_(filtered_input);

	auto freq = freq_detector_({ float(SR()) }, zero_crossings);

	freq = freq_smoother_(freq, 0.001f * SR());

//	freq *= 1.0f - peaks;

	for (int c = 0; c < 2; c++)
	{
		out_vec.row(c) = sine_[c](freq.constRow(c) / float(SR()));
	}

	out_vec *= peaks;
 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
}

} // splatter
