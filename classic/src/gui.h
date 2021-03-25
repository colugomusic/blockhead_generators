#pragma once

#include <blink/standard_traversers/classic.hpp>
#include <blink_sampler.h>

class Classic;

class GUI
{
public:

	blink_Error get_waveform_positions(const Classic* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp);

private:

	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	blink::std_traversers::Classic position_traverser_;

};