#pragma once

#include <blink/standard_parameters/loop.hpp>

namespace classic {
namespace params {
namespace loop {

static constexpr auto UUID { blink::std_params::loop::UUID };

auto inline toggle() { return blink::std_params::loop::toggle(); }

} // loop
} // params
} // classic
