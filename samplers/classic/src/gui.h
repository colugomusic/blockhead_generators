#pragma once

#include <blink/standard_traversers/classic.hpp>
#include <blink_sampler.h>

namespace classic {

class Plugin;

class GUI
{
public:

	blink_Error draw(const classic::Plugin& plugin, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out);

private:

	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	blink::std_traversers::Classic position_traverser_;

};

} // classic