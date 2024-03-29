#pragma once

#include <blink/standard_parameters/scale.hpp>

namespace fudge {
namespace params {
namespace harmonics_scale {

static constexpr auto UUID { blink::std_params::scale::UUID };

auto inline chord()
{
	auto out { blink::std_params::scale::chord() };

	out.flags |= blink_ChordFlags_CanManipulate;
	out.flags |= blink_ChordFlags_IconOnly;
	out.flags |= blink_ChordFlags_AlwaysShowButtonWhenGroupIsVisible;

	return out;
}

} // harmonics_scale
} // params
} // fudge
