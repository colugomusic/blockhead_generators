#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(RingModulator* plugin, int instance_group)
	: Effect(plugin, instance_group)
	, plugin_(plugin)
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	struct Data
	{
		const blink_EnvelopeData* env_pitch;
		const blink_EnvelopeData* env_mix;
	} data;

	data.env_pitch = plugin_->get_envelope_data(buffer->parameter_data, int(RingModulator::ParameterIndex::Env_Pitch));
	data.env_mix = plugin_->get_envelope_data(buffer->parameter_data, int(RingModulator::ParameterIndex::Env_Mix));

	const auto pitch = plugin_->env_pitch().search_vec(data.env_pitch, block_positions()) + 60.0f;
	const auto mix = plugin_->env_mix().search_vec(data.env_mix, block_positions());

	const auto omega = math::convert::pitch_to_frequency(pitch) / float(buffer->sample_rate);
	const auto sine = sine_(omega);
	const auto am = (sine + 1.0f) / 2.0f;
	const auto out_vec = ml::DSPVectorArray<2>(in) * ml::repeatRows<2>(ml::lerp(ml::DSPVector(1.0f), am, mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	sine_.clear();
}
