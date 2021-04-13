#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(const Test* plugin)
	: plugin_(plugin)
	, prev_pos_(std::numeric_limits<float>::max())
{
}

blink_Error Audio::process(const blink_SynthBuffer* buffer, float* out)
{
	ml::DSPVector block_positions;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		block_positions[i] = float(buffer->positions[i] - buffer->data_offset);
	}

	struct Data
	{
		const blink_EnvelopeData* env_wave;
		const blink_EnvelopeData* env_p0;
		const blink_EnvelopeData* env_p1;
		const blink_EnvelopeData* env_fm0;
		const blink_EnvelopeData* env_fm1;
	} data;

	data.env_wave = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Wave));
	data.env_p0 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Pitch0));
	data.env_p1 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Pitch1));
	data.env_fm0 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_FM0));
	data.env_fm1 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_FM1));

	const auto wave = plugin_->env_wave().search_vec(data.env_wave, block_positions, prev_pos_);
	const auto p0 = plugin_->env_p0().search_vec(data.env_p0, block_positions, prev_pos_) + 60.0f;
	const auto p1 = plugin_->env_p1().search_vec(data.env_p1, block_positions, prev_pos_) + 60.0f;
	const auto fm0 = plugin_->env_fm0().search_vec(data.env_fm0, block_positions, prev_pos_);
	const auto fm1 = plugin_->env_fm1().search_vec(data.env_fm1, block_positions, prev_pos_);
	
	const auto freq0 = blink::math::convert::pitch_to_frequency(p0);
	const auto freq1 = blink::math::convert::pitch_to_frequency(p1);

	ml::DSPVector osc_out;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		const auto fm0_freq = std::max(((fm0[i] * fm0[i] * 20000.0f) * oscs_[1].value()) + freq0[i], 0.0f);
		const auto fm1_freq = std::max(((fm1[i] * fm1[i] * 20000.0f) * oscs_[0].value()) + freq1[i], 0.0f);

		osc_out[i] = oscs_[0](fm0_freq / buffer->sample_rate, 0.5f, wave[i]);

		oscs_[1](fm1_freq / buffer->sample_rate, 0.5f, bleat::oscillators::scalar::MultiWaveOsc::WAVE_SINE);
	}

	const auto out_vec = ml::repeatRows<2>(osc_out);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	prev_pos_ = block_positions[kFloatsPerDSPVector - 1];

	return BLINK_OK;
}

blink_Error Audio::reset()
{
	oscs_[0].reset();
	oscs_[1].reset();

	return BLINK_OK;
}
