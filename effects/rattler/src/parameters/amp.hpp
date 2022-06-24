#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace rattler {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::amp::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags &= ~blink_EnvelopeFlags_MovesDisplay;

	return out;
}

auto inline slider_parameter() { return blink::std_params::amp::slider_parameter(); }
auto inline envelope_manipulator_target() { return blink::std_params::amp::envelope_manipulator_target(); }

} // amp
} // params
} // rattler
