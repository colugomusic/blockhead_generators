#pragma once

#include <blink/standard_parameters/reverse.hpp>
#include <blink/parameters/option_spec.hpp>

namespace classic {
namespace params {
namespace reverse {

static constexpr auto UUID { blink::std_params::reverse::UUID };

auto inline option()
{
	blink::OptionSpec out;

	out.uuid = UUID;
	out.default_index = 0;
	out.name = "Reverse";
	out.flags =
		blink_OptionFlags_IsManipulatorTarget |
		blink_OptionFlags_MovesDisplay |
		blink_OptionFlags_Hidden;

	out.options = {
		"Mirror",
		"Tape",
		"Slip",
	};

	return out;
}

auto inline toggle()
{
	auto out { blink::std_params::reverse::toggle() };

	out.flags |= blink_ToggleFlags_CanManipulate;

	return out;
}

} // reverse
} // params
} // classic
