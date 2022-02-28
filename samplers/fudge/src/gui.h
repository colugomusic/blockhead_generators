#pragma once

#include <blink/standard_traversers/fudge.hpp>
#include <blink_sampler.h>

namespace fudge {

class Plugin;

class GUI
{
public:

	blink_Error draw(const Plugin& plugin, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out);

private:

	blink::Traverser block_traverser_;
	blink::std_traversers::Fudge position_traverser_;

};

}
