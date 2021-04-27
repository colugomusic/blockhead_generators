#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(const Test* plugin)
	: plugin_(plugin)
{
}

blink_Error Audio::process(const blink_SynthBuffer* buffer, float* out)
{
	Synth::begin_process(buffer);

	struct Data
	{
		const blink_EnvelopeData* env_amp;
		const blink_EnvelopeData* env_wave;
		const blink_EnvelopeData* env_p0;
		const blink_EnvelopeData* env_p1;
		const blink_EnvelopeData* env_fm0;
		const blink_EnvelopeData* env_fm1;
		const blink_OptionData* option_noise_mode;
		const blink_EnvelopeData* env_noise_amount;
		const blink_EnvelopeData* env_noise_color;
		const blink_SliderData* slider_noise_width;
	} data;

	data.env_amp = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Amp));
	data.env_wave = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Wave));
	data.env_p0 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Pitch0));
	data.env_p1 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_Pitch1));
	data.env_fm0 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_FM0));
	data.env_fm1 = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_FM1));
	data.option_noise_mode = plugin_->get_option_data(buffer->parameter_data, int(Test::ParameterIndex::Option_NoiseMode));
	data.env_noise_amount = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_NoiseAmount));
	data.env_noise_color = plugin_->get_envelope_data(buffer->parameter_data, int(Test::ParameterIndex::Env_NoiseColor));
	data.slider_noise_width = plugin_->get_slider_data(buffer->parameter_data, int(Test::ParameterIndex::Sld_NoiseWidth));

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions());
 	const auto wave = plugin_->env_wave().search_vec(data.env_wave, block_positions());
	const auto p0 = plugin_->env_p0().search_vec(data.env_p0, block_positions()) + 60.0f;
	const auto p1 = plugin_->env_p1().search_vec(data.env_p1, block_positions()) + 60.0f;
	const auto fm0 = plugin_->env_fm0().search_vec(data.env_fm0, block_positions());
	const auto fm1 = plugin_->env_fm1().search_vec(data.env_fm1, block_positions());
	
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

	auto out_vec = ml::repeatRows<2>(osc_out);

	out_vec =
		noise_gen_(
			out_vec,
			data.option_noise_mode->index,
			plugin_->env_noise_amount(),
			plugin_->env_noise_color(),
			*data.env_noise_amount,
			*data.env_noise_color,
			*data.slider_noise_width,
			block_positions());

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

blink_Error Audio::reset()
{
	oscs_[0].reset();
	oscs_[1].reset();

	return BLINK_OK;
}
