#pragma once

#include <blink/standard_parameters/pan.hpp>

namespace test {
namespace params {
namespace pan {

static constexpr auto UUID { blink::std_params::pan::UUID };

auto inline envelope_parameter() { return blink::std_params::pan::envelope_parameter(); }
auto inline slider_parameter() { return blink::std_params::pan::slider_parameter(); }
auto inline envelope_manipulator_target() { return blink::std_params::pan::envelope_manipulator_target(); }

} // pan
} // params
} // test
