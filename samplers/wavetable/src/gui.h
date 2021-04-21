#pragma once

#include <blink/standard_traversers/fudge.hpp>
#include <blink_sampler.h>

class Wavetable;

class GUI
{
public:

	blink_Error draw(const Wavetable* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out);

private:

	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	blink::std_traversers::Fudge position_traverser_;

};