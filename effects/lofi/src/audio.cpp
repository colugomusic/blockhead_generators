#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace lofi {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
	reset();
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const blink_ParameterData* parameter_data, const float* in, float* out)
{
	AudioData data(plugin_, parameter_data);

	const auto sr = data.envelopes.SR.search_vec(block_positions());
	const auto br = data.envelopes.BR.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	const auto sr_to_inc = [](float SR, ml::DSPVector x)
	{
		x = (0.99f * (x * x * x * x)) + 0.01f;

		return (x * SR) / SR;
	};

	const auto inc = sr_to_inc(float(SR()), sr);

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		phase_ += inc[i];

		if (phase_ >= 0.5f)
		{
			value_[0] = in_vec.constRow(0)[i];
			value_[1] = in_vec.constRow(1)[i];
			phase_ -= 1.0f;
		}

		out_vec.row(0)[i] = value_[0];
		out_vec.row(1)[i] = value_[1];
	}

	const auto br_to_step = [](ml::DSPVector x)
	{
		x -= 1;

		return (x * x * x * x) * 0.95f;
	};

	const auto step = br_to_step(br);

	out_vec = math::stepify(out_vec, ml::repeatRows<2>(step));

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	phase_ = 0.0f;
	value_[0] = 0.0f;
	value_[1] = 0.0f;
}

} // lofi
