#pragma once

#include <blink/standard_parameters/noise_width.hpp>

namespace classic {
namespace params {
namespace noise_width {

static constexpr auto UUID { blink::std_params::noise_width::UUID };

auto inline slider_parameter()
{
	auto out { blink::std_params::noise_width::slider_parameter() };

	out.flags |= blink_SliderFlags_NonGlobal;

	return out;
}

auto inline envelope_manipulator_target() { return blink::std_params::noise_width::envelope_manipulator_target(); }

} // noise_width
} // params
} // classic
