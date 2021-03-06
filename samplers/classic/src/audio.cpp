#include "audio.h"
#include "plugin.h"
#include "instance.h"

using namespace blink;

namespace classic {

Audio::Audio(Instance* instance)
	: SamplerUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_SamplerBuffer* buffer, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	block_traverser_.generate(block_positions(), kFloatsPerDSPVector);

	struct Data
	{
		const blink_OptionData* option_noise_mode;
		const blink_EnvelopeData* env_amp;
		const blink_EnvelopeData* env_pan;
		const blink_EnvelopeData* env_pitch;
		const blink_EnvelopeData* env_noise_amount;
		const blink_EnvelopeData* env_noise_color;
		const blink_SliderData* slider_amp;
		const blink_SliderData* slider_pan;
		const blink_SliderData* slider_pitch;
		const blink_IntSliderData* slider_sample_offset;
		const blink_SliderData* slider_noise_width;
		const blink_ToggleData* toggle_loop;
		const blink_ToggleData* toggle_reverse;
	} data;

	data.option_noise_mode    = plugin_->get_option_data(buffer->parameter_data, int(Plugin::ParameterIndex::Option_NoiseMode));
	data.env_amp              = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Amp));
	data.env_pan              = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Pan));
	data.env_pitch            = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Pitch));
	data.env_noise_amount     = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_NoiseAmount));
	data.env_noise_color      = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_NoiseColor));
	data.slider_amp           = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Amp));
	data.slider_pan           = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Pan));
	data.slider_pitch         = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_Pitch));
	data.slider_sample_offset = plugin_->get_int_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_SampleOffset));
	data.slider_noise_width   = plugin_->get_slider_data(buffer->parameter_data, int(Plugin::ParameterIndex::Sld_NoiseWidth));
	data.toggle_loop          = plugin_->get_toggle_data(buffer->parameter_data, int(Plugin::ParameterIndex::Tog_Loop));
	data.toggle_reverse       = plugin_->get_toggle_data(buffer->parameter_data, int(Plugin::ParameterIndex::Tog_Reverse));

	traverser_resetter_.check(data.env_pitch, &block_traverser_);

	auto sample_pos = position_traverser_.get_positions(data.slider_pitch->value, data.env_pitch, block_traverser_, data.slider_sample_offset->value, kFloatsPerDSPVector);

	sample_pos /= float(buffer->song_rate) / buffer->sample_info->SR;

	SampleData sample_data(buffer->sample_info, buffer->channel_mode);

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions()) * data.slider_amp->value;

	if (data.toggle_reverse->value)
	{
		sample_pos = std::int32_t(buffer->sample_info->num_frames - 1) - sample_pos;
	}

	if (buffer->sample_info->num_channels > 1)
	{
		out_vec = process_stereo_sample(sample_data, sample_pos, data.toggle_loop->value);
	}
	else
	{
		out_vec = process_mono_sample(sample_data, sample_pos, data.toggle_loop->value);
	}

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
	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

ml::DSPVectorArray<2> Audio::process_stereo_sample(const SampleData& sample_data, const snd::transport::DSPVectorFramePosition &sample_pos, bool loop)
{
	ml::DSPVectorArray<2> out;

	switch (sample_data.get_channel_mode())
	{
		default:
		case blink_ChannelMode_Stereo:
		{
			return sample_data.read_frames_interp<2>(sample_pos, loop);
		}

		case blink_ChannelMode_Left:
		{
			return ml::repeatRows<2>(sample_data.read_frames_interp(0, sample_pos, loop));
		}

		case blink_ChannelMode_Right:
		{
			return ml::repeatRows<2>(sample_data.read_frames_interp(1, sample_pos, loop));
		}
	}
}

ml::DSPVectorArray<2> Audio::process_mono_sample(const SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop)
{
	return ml::repeatRows<2>(sample_data.read_frames_interp(0, sample_pos, loop));
}

blink_Error Audio::preprocess_sample(void* host, blink_PreprocessCallbacks callbacks) const
{
	return BLINK_OK;
}

} // classic