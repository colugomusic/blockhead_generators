#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace resonator {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
	reset();
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	const auto pitch = data.envelopes.pitch.search_vec(block_positions());
	auto dispersion = data.envelopes.dispersion.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	const auto frequency = math::convert::pitch_to_frequency(60.0f + pitch);
	const auto omega = frequency / float(SR());

	dispersion = 0.5f * (ml::powApprox(ml::DSPVector(0.0001f), 1.0 - dispersion));

	for (int c = 0; c < 2; c++)
	{
		const auto& in_row = in_vec.constRow(c);
		auto& out_row = out_vec.row(c);

		for (int i = 0; i < kFloatsPerDSPVector; i++)
		{
			velocity_[c] -= (position_[c] - in_row[i]) * omega[i];
			velocity_[c] *= (1.0f - dispersion[i]);
			position_[c] += velocity_[c];
			position_[c] = std::clamp(position_[c], -1.0f, 1.0f);

			out_row[i] = position_[c];
		}
	}

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	velocity_.fill(0.0f);
	position_.fill(0.0f);
}

} // resonator
