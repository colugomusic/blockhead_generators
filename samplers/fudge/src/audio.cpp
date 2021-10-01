#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace fudge {

Audio::Audio(Instance* instance)
	: SamplerUnit(instance)
	, plugin_(instance->get_plugin())
	, controller_(plugin_)
	, particles_{ controller_, { controller_, 1 }, { controller_, 2 }, { controller_, 3 } }
{
}

blink_Error Audio::process(const blink_SamplerBuffer* buffer, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	block_traverser_.generate(block_positions(), kFloatsPerDSPVector);

	AudioData data;

	data.option_noise_mode     = plugin_->get_option_data(buffer->parameter_data, int(Plugin::ParameterIndex::Option_NoiseMode));
	data.chord_harmonics_scale = plugin_->get_chord_data(buffer->parameter_data, int(Plugin::ParameterIndex::Chord_Harmonics_Scale));
	data.env_amp               = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Amp));
	data.env_pan               = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Pan));
	data.env_pitch             = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Pitch));
	data.env_speed             = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Speed));
	data.env_grain_size        = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_GrainSize));
	data.env_grain_transpose   = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_GrainTranspose));
	data.env_uniformity        = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Uniformity));
	data.env_harmonics_amount  = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Harmonics_Amount));
	data.env_harmonics_spread  = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Harmonics_Spread));
	data.env_noise_amount      = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_NoiseAmount));
	data.env_noise_color       = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_NoiseColor));
	data.slider_amp            = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Amp));
	data.slider_pan            = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Pan));
	data.slider_pitch          = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Pitch));
	data.slider_speed          = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Speed));
	data.slider_sample_offset  = plugin_->get_int_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_SampleOffset));
	data.slider_noise_width    = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_NoiseWidth));
	data.toggle_loop           = plugin_->get_toggle_data(buffer->parameter_data, int(Plugin::ParameterIndex::Tog_Loop));
	data.toggle_reverse        = plugin_->get_toggle_data(buffer->parameter_data, int(Plugin::ParameterIndex::Tog_Reverse));
	data.warp_points           = buffer->warp_points;

	traverser_resetter_.check(data.env_speed, &block_traverser_);

	const auto analysis_data = buffer->analysis_ready ? plugin_->get_analysis_data(buffer->sample_info->id) : nullptr;

	controller_.process(data, *buffer, analysis_data, block_traverser_, block_positions(), SR());

	const auto harmonic_amount = plugin_->env_harmonics_amount().search_vec(data.env_harmonics_amount, block_positions());

	ml::DSPVector total_amp;

	for (int i = 0; i < 4; i++)
	{
		auto& particle = particles_[i];

		ml::DSPVector amp;

		if (i == 0)
		{
			amp = 1.0f;
		}
		else
		{
			amp = ml::clamp(harmonic_amount - float(i - 1), ml::DSPVector(0.0f), ml::DSPVector(1.0f));
		}

		total_amp += amp;
		out_vec += particles_[i].process(amp);
	}

	out_vec /= ml::repeatRows<2>(total_amp);

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

	out_vec = stereo_pan(out_vec, data.slider_pan->value, plugin_->env_pan(), data.env_pan, block_positions());

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions()) * data.slider_amp->value;

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

}
