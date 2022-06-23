#pragma once

#include <blink/standard_parameters/delay_time.hpp>

namespace echo {
namespace params {
namespace time {

static constexpr auto UUID{ blink::std_params::delay_time::UUID };

auto inline envelope_parameter()
{
	return blink::std_params::delay_time::envelope_parameter();
}

auto inline slider_parameter()
{
	return blink::std_params::delay_time::slider_parameter();
}

auto inline envelope_manipulator_target()
{
	return blink::std_params::delay_time::envelope_manipulator_target();
}

} // time
} // params
} // echo
