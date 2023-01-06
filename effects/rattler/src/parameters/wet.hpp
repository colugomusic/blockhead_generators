#pragma once

#include <blink/standard_parameters/wet.hpp>

namespace rattler {
namespace params {
namespace wet {

static constexpr auto UUID { blink::std_params::wet::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::wet::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter()
{
	auto out{ blink::std_params::wet::slider_parameter() };

	out.slider.default_value = 1;

	return out;
}

} // wet
} // params
} // rattler
