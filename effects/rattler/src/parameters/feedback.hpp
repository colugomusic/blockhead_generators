#pragma once

#include <blink/standard_parameters/feedback.hpp>

namespace rattler {
namespace params {
namespace feedback {

static constexpr auto UUID { blink::std_params::feedback::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::feedback::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter() { return blink::std_params::feedback::slider_parameter(); }
auto inline envelope_manipulator_target() { return blink::std_params::feedback::envelope_manipulator_target(); }

} // feedback
} // params
} // rattler