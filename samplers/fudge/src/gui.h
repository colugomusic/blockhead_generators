#pragma once

#include <blink/standard_traversers/fudge.hpp>
#include <blink_sampler.h>

class Fudge;

class GUI
{
public:

	blink_Error get_waveform_positions(const Fudge* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp);

private:

	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	blink::std_traversers::Fudge position_traverser_;

};