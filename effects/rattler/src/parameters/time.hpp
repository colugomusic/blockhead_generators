#pragma once

#include <blink/standard_parameters/delay_time.hpp>

namespace rattler {
namespace params {
namespace time {

static constexpr auto UUID{ blink::std_params::delay_time::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::delay_time::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline envelope_manipulator_target()
{
	return blink::std_params::delay_time::envelope_manipulator_target();
}

} // time
} // params
} // rattler
