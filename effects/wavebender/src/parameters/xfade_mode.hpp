#pragma once

#include <blink/parameters/option_spec.hpp>

namespace wavebender {
namespace params {
namespace xfade_mode {

static constexpr auto UUID { "32e0b599-2dc9-4dfc-b7fa-e284e72289b5" };

inline auto option()
{
	blink::OptionSpec out;

	out.uuid = UUID;
	out.name = "Crossfade Mode";

	out.default_index = 1;
	out.options.push_back("Static");
	out.options.push_back("Dynamic");

	out.flags |= blink_OptionFlags_CanManipulate;
	out.flags |= blink_OptionFlags_IsManipulatorTarget;

	return out;
}

} // xfade_mode
} // params
} // wavebender
