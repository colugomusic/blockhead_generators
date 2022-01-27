#pragma once

#include <blink/standard_parameters/reverse.hpp>

namespace fudge{
namespace params {
namespace reverse {

static constexpr auto UUID { blink::std_params::reverse::UUID };

auto inline toggle()
{
	auto out { blink::std_params::reverse::toggle() };

	out.flags |= blink_ToggleFlags_CanManipulate;
	out.flags |= blink_ToggleFlags_IsManipulatorTarget;

	return out;
}

} // reverse
} // params
} // fudge
