#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace saturator {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

inline auto slider_parameter() { return blink::std_params::amp::slider_parameter(); }

inline auto envelope_parameter()
{
	auto out = blink::std_params::amp::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline envelope_manipulator_target()
{
	return blink::std_params::amp::envelope_manipulator_target();
}

} // amp
} // params
} // saturator
