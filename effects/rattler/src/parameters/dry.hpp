#pragma once

#include <blink/standard_parameters/dry.hpp>

namespace rattler {
namespace params {
namespace dry {

static constexpr auto UUID { blink::std_params::dry::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::dry::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter()
{
	auto out{ blink::std_params::dry::slider_parameter() };

	out.slider.default_value = 1;

	return out;
}


} // dry
} // params
} // rattler
