#include "gui.h"
#include "plugin.h"

using namespace blkhdgen;

blkhdgen_Error GUI::get_waveform_positions(const Classic* plugin, const blkhdgen_SamplerBuffer* buffer, blkhdgen_FrameCount n, float* out, float* derivatives)
{
	gui_block_traverser_.set_reset(0);

	const auto pitch_slider_data = buffer->parameter_data[int(Classic::ParameterIndex::Sld_Pitch)].slider;
	const auto pitch_envelope_data = buffer->parameter_data[int(Classic::ParameterIndex::Env_Pitch)].envelope;
	const auto sample_offset_slider_data = buffer->parameter_data[int(Classic::ParameterIndex::Sld_SampleOffset)].int_slider;

	auto frames_remaining = n;
	int index = 0;

	while (frames_remaining > 0)
	{
		gui_block_traverser_.generate(buffer->positions + index, buffer->data_offset);

		ml::DSPVector derivatives_vec;

		const auto positions =
			gui_position_traverser_.get_positions(
				pitch_slider_data.value,
				plugin->env_pitch(),
				&pitch_envelope_data.points,
				&gui_block_traverser_,
				sample_offset_slider_data.value,
				derivatives_vec.getBuffer());

		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		std::copy(positions.getConstBuffer(), positions.getConstBuffer() + count, out + index);

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLKHDGEN_OK;
}