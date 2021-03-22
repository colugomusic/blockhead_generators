#include "gui.h"
#include "plugin.h"

using namespace blink;

blink_Error GUI::get_waveform_positions(const Classic* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp)
{
	gui_block_traverser_.set_reset(0);

	const auto data_slider_amp           = Plugin::get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Amp));
	const auto data_slider_pitch         = Plugin::get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Pitch));
	const auto data_envelope_pitch       = Plugin::get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Pitch));
	const auto data_slider_sample_offset = Plugin::get_int_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_SampleOffset));
	const auto data_toggle_loop          = Plugin::get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Loop));
	const auto data_toggle_reverse       = Plugin::get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Reverse));

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

		if (amp && data_slider_amp)
		{
			ml::DSPVector amp_vec(data_slider_amp->value);

			std::copy(amp_vec.getConstBuffer(), amp_vec.getConstBuffer() + count, amp + index);
		}

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}