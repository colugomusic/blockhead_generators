#pragma once

#include <blink/standard_parameters/feedback.hpp>

namespace resonator {
namespace params {
namespace feedback {

static constexpr auto UUID { blink::std_params::feedback::UUID };

inline auto envelope_parameter()
{
	auto out { blink::std_params::feedback::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

inline auto envelope_manipulator_target()
{
	return blink::std_params::feedback::envelope_manipulator_target();
}

} // feedback
} // params
} // resonator