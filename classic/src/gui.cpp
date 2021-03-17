#include "gui.h"
#include "plugin.h"

using namespace blkhdgen;

blkhdgen_Error GUI::get_waveform_positions(const Classic* plugin, const blkhdgen_SamplerBuffer* buffer, blkhdgen_FrameCount n, float* out, float* derivatives)
{
	gui_block_traverser_.set_reset(0);

	const auto data_slider_pitch = buffer->parameter_data ? &buffer->parameter_data[int(Classic::ParameterIndex::Sld_Pitch)].slider : nullptr;
	const auto data_envelope_pitch = buffer->parameter_data ? &buffer->parameter_data[int(Classic::ParameterIndex::Env_Pitch)].envelope : nullptr;
	const auto data_slider_sample_offset = buffer->parameter_data ? &buffer->parameter_data[int(Classic::ParameterIndex::Sld_SampleOffset)].int_slider : nullptr;
	const auto data_toggle_loop = buffer->parameter_data ? &buffer->parameter_data[int(Classic::ParameterIndex::Tog_Loop)].toggle : nullptr;
	const auto data_toggle_reverse = buffer->parameter_data ? &buffer->parameter_data[int(Classic::ParameterIndex::Tog_Reverse)].toggle : nullptr;

	auto frames_remaining = n;
	int index = 0;

	while (frames_remaining > 0 && frames_remaining <= buffer->sample_info->num_frames)
	{
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		gui_block_traverser_.generate(buffer->positions + index, count, buffer->data_offset);

		ml::DSPVector derivatives_vec;

		auto positions =
			gui_position_traverser_.get_positions(
				data_slider_pitch ? data_slider_pitch->value : 0,
				plugin->env_pitch(),
				data_envelope_pitch ? &data_envelope_pitch->points : nullptr,
				&gui_block_traverser_,
				data_slider_sample_offset ? data_slider_sample_offset->value : 0,
				derivatives ? derivatives_vec.getBuffer() : nullptr);

		//ml::validate(positions);

		if (data_toggle_loop && data_toggle_loop->value)
		{
			for (int i = 0; i < kFloatsPerDSPVector; i++)
			{
				if (positions[i] > buffer->sample_info->num_frames - 1)
				{
					positions[i] = float(std::fmod(positions[i], buffer->sample_info->num_frames));
				}
			}
		}

		if (data_toggle_reverse && data_toggle_reverse->value)
		{
			positions = float(buffer->sample_info->num_frames - 1) - positions;
		}

		positions /= (float(buffer->song_rate) / buffer->sample_info->SR);

		std::copy(positions.getConstBuffer(), positions.getConstBuffer() + count, out + index);
		if (derivatives) std::copy(derivatives_vec.getConstBuffer(), derivatives_vec.getConstBuffer() + count, derivatives + index);

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLKHDGEN_OK;
}