#pragma once

#include <blink/standard_parameters/pan.hpp>

namespace berk {
namespace params {
namespace pan {

static constexpr auto UUID { blink::std_params::pan::UUID };

inline auto envelope_parameter() { return blink::std_params::pan::envelope_parameter(); }
inline auto slider_parameter() { return blink::std_params::pan::slider_parameter(); }
inline auto envelope_manipulator_target() { return blink::std_params::pan::envelope_manipulator_target(); }

} // amp
} // params
} // berk
