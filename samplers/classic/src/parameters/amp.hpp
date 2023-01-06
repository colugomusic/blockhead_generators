#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace classic {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::amp::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter() { return blink::std_params::amp::slider_parameter(); }

} // amp
} // params
} // classic
