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
		blkhdgen_EnvelopeData env_amp;
		blkhdgen_EnvelopeData env_pan;
		blkhdgen_EnvelopeData env_pitch;
		blkhdgen_SliderData slider_pan;
		blkhdgen_SliderData slider_pitch;
		blkhdgen_IntSliderData slider_sample_offset;
		blkhdgen_ToggleData toggle_loop;
		blkhdgen_ToggleData toggle_reverse;
	} data;

	data.env_amp = buffer->parameter_data[int(Classic::ParameterIndex::Env_Amp)].envelope;
	data.env_pan = buffer->parameter_data[int(Classic::ParameterIndex::Env_Pan)].envelope;
	data.env_pitch = buffer->parameter_data[int(Classic::ParameterIndex::Env_Pitch)].envelope;
	data.slider_pan = buffer->parameter_data[int(Classic::ParameterIndex::Sld_Pan)].slider;
	data.slider_pitch = buffer->parameter_data[int(Classic::ParameterIndex::Sld_Pitch)].slider;
	data.slider_sample_offset = buffer->parameter_data[int(Classic::ParameterIndex::Sld_SampleOffset)].int_slider;
	data.toggle_loop = buffer->parameter_data[int(Classic::ParameterIndex::Tog_Loop)].toggle;
	data.toggle_reverse = buffer->parameter_data[int(Classic::ParameterIndex::Tog_Reverse)].toggle;

	auto sample_pos = position_traverser_.get_positions(data.slider_pitch.value, plugin_->env_pitch(), &data.env_pitch.points, &block_traverser_, data.slider_sample_offset.value);

	sample_pos /= float(buffer->song_rate) / buffer->sample_info->SR;

	SampleData sample_data(buffer->sample_info);

	const auto amp = plugin_->env_amp().get_mod_values(&block_traverser_, &data.env_amp.points);

	if (data.toggle_reverse.value)
	{
		sample_pos = float(buffer->sample_info->num_frames - 1) - sample_pos;
	}

	if (buffer->sample_info->num_channels > 0)
	{
		out_vec.row(0) = sample_data.read_frames_interp(0, sample_pos, data.toggle_loop.value);
	}

	if (buffer->sample_info->num_channels > 1)
	{
		out_vec.row(1) = sample_data.read_frames_interp(1, sample_pos, data.toggle_loop.value);
	}
	else
	{
		out_vec.row(1) = out_vec.row(0);
	}

	out_vec = stereo_pan(out_vec, data.slider_pan.value, plugin_->env_pan(), &data.env_pan.points, &block_traverser_);

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLKHDGEN_OK;
}

blkhdgen_Error Audio::preprocess_sample(void* host, blkhdgen_PreprocessCallbacks callbacks) const
{
	return BLKHDGEN_OK;
}