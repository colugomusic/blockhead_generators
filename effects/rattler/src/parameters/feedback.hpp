#pragma once

#include <blink/standard_parameters/feedback.hpp>
#include "../parameters.h"

namespace rattler {
namespace params {
namespace feedback {

static constexpr auto UUID { blink::std_params::feedback::UUID };

auto inline envelope_parameter() {
	auto out { blink::std_params::feedback::envelope_parameter() };
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	return out;
}

auto inline slider_parameter() {
	auto out{blink::std_params::feedback::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Feedback);
	return out;
}

} // feedback
} // params
} // rattler
