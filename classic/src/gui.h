#pragma once

#include <blkhdgen/standard_traversers/classic.hpp>
#include <blkhdgen_sampler.h>

class Classic;

class GUI
{
public:

	blkhdgen_Error get_waveform_positions(const Classic* plugin, const blkhdgen_SamplerBuffer* buffer, blkhdgen_FrameCount n, float* out, float* derivatives);

private:

	blkhdgen::Traverser gui_block_traverser_;
	blkhdgen::std_traversers::Classic gui_position_traverser_;

};