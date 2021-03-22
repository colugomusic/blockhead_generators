#pragma once

#include <blink/standard_traversers/classic.hpp>
#include <blink_sampler.h>

class Classic;

class GUI
{
public:

	blink_Error get_waveform_positions(const Classic* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp);

private:

	blink::Traverser gui_block_traverser_;
	blink::std_traversers::Classic gui_position_traverser_;

};