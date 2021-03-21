#include "audio.h"
#include "plugin.h"

using namespace blkhdgen;

Audio::Audio(const Classic* plugin)
	: plugin_(plugin)
{
}

blkhdgen_Error Audio::process(const blkhdgen_SamplerBuffer* buffer, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	block_traverser_.generate(buffer->positions, kFloatsPerDSPVector, buffer->data_offset);

	struct Data
	{
		const blkhdgen_EnvelopeData* env_amp;
		const blkhdgen_EnvelopeData* env_pan;
		const blkhdgen_EnvelopeData* env_pitch;
		const blkhdgen_SliderData* slider_amp;
		const blkhdgen_SliderData* slider_pan;
		const blkhdgen_SliderData* slider_pitch;
		const blkhdgen_IntSliderData* slider_sample_offset;
		const blkhdgen_ToggleData* toggle_loop;
		const blkhdgen_ToggleData* toggle_reverse;
	} data;

	data.env_amp              = plugin_->get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Amp));
	data.env_pan              = plugin_->get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Pan));
	data.env_pitch            = plugin_->get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Pitch));
	data.slider_amp           = plugin_->get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Amp));
	data.slider_pan           = plugin_->get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Pan));
	data.slider_pitch         = plugin_->get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Pitch));
	data.slider_sample_offset = plugin_->get_int_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_SampleOffset));
	data.toggle_loop          = plugin_->get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Loop));
	data.toggle_reverse       = plugin_->get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Reverse));

	auto sample_pos = position_traverser_.get_positions(data.slider_pitch->value, &data.env_pitch->points, &block_traverser_, data.slider_sample_offset->value);

	sample_pos /= float(buffer->song_rate) / buffer->sample_info->SR;

	SampleData sample_data(buffer->sample_info);

	const auto amp = plugin_->env_amp().get_mod_values(&block_traverser_, data.env_amp) * data.slider_amp->value;

	if (data.toggle_reverse->value)
	{
		sample_pos = float(buffer->sample_info->num_frames - 1) - sample_pos;
	}

	if (buffer->sample_info->num_channels > 0)
	{
		out_vec.row(0) = sample_data.read_frames_interp(0, sample_pos, data.toggle_loop->value);
	}

	if (buffer->sample_info->num_channels > 1)
	{
		out_vec.row(1) = sample_data.read_frames_interp(1, sample_pos, data.toggle_loop->value);
	}
	else
	{
		out_vec.row(1) = out_vec.row(0);
	}

	out_vec = stereo_pan(out_vec, data.slider_pan->value, plugin_->env_pan(), data.env_pan, &block_traverser_);

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLKHDGEN_OK;
}

blkhdgen_Error Audio::preprocess_sample(void* host, blkhdgen_PreprocessCallbacks callbacks) const
{
	return BLKHDGEN_OK;
}