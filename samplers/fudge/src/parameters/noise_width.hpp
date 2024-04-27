#pragma once

#include <blink/standard_parameters/noise_width.hpp>

namespace fudge {
namespace params {
namespace noise_width {

static constexpr auto UUID { blink::std_params::noise_width::UUID };

auto inline slider_parameter() {
	return blink::std_params::noise_width::slider_parameter();
}

} // noise_width
} // params
} // fudge
