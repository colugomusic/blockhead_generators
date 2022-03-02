#pragma once

#include <blink/standard_parameters/mix.hpp>

namespace zap {
namespace params {
namespace mix {

static constexpr auto UUID { blink::std_params::mix::UUID };

auto inline envelope_parameter() { return blink::std_params::mix::envelope_parameter(); }
auto inline envelope_manipulator_target() { return blink::std_params::mix::envelope_manipulator_target(); }

} // mix
} // params
} // zap
