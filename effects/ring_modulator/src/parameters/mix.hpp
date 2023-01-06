#pragma once

#include <blink/standard_parameters/mix.hpp>

namespace ring_modulator {
namespace params {
namespace mix {

static constexpr auto UUID { blink::std_params::mix::UUID };

auto inline envelope_parameter() { return blink::std_params::mix::envelope_parameter(); }

} // mix
} // params
} // ring_modulator
