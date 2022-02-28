#pragma once

#include <blink/transform/tape.hpp>
#include <blink_sampler.h>

namespace classic {

class Plugin;

class GUI
{
public:

	blink_Error draw(const classic::Plugin& plugin, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out);

private:

	blink::transform::Tape tape_transformer_;

};

} // classic