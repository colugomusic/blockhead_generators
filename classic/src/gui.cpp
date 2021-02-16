#include "gui.h"
#include "plugin.h"

using namespace blkhdgen;

blkhdgen_Error GUI::get_waveform_positions(const Classic* plugin, const blkhdgen_ParameterData* parameter_data, int data_offset, const blkhdgen_Position* block_positions, float* out, float* derivatives)
{
	gui_block_traverser_.generate(block_positions, data_offset);

	const auto pitch_slider_data = parameter_data[int(Classic::ParameterIndex::Sld_Pitch)].slider;
	const auto pitch_envelope_data = parameter_data[int(Classic::ParameterIndex::Env_Pitch)].envelope;
	const auto sample_offset_slider_data = parameter_data[int(Classic::ParameterIndex::Sld_SampleOffset)].int_slider;

	const auto positions = gui_position_traverser_.get_positions(pitch_slider_data.value, plugin->env_pitch(), &pitch_envelope_data.points, &gui_block_traverser_, sample_offset_slider_data.value, derivatives);

	ml::storeAligned(positions, out);

	return BLKHDGEN_OK;
}