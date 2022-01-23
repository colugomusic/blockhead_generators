#pragma once

#include <blink/standard_parameters/noise_mode.hpp>

namespace classic {
namespace params {
namespace noise_mode {

static constexpr auto UUID { blink::std_params::noise_mode::UUID };

auto inline option() { return blink::std_params::noise_mode::option(); }

} // noise_mode
} // params
} // classic
