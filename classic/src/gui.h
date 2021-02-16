#pragma once

#include <blkhdgen/standard_traversers/classic.hpp>

class Classic;

class GUI
{
public:

	blkhdgen_Error get_waveform_positions(const Classic* plugin, const blkhdgen_ParameterData* parameter_data, int data_offset, const blkhdgen_Position* block_positions, float* out, float* derivatives = nullptr);

private:

	blkhdgen::Traverser gui_block_traverser_;
	blkhdgen::std_traversers::Classic gui_position_traverser_;

};