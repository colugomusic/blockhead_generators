#pragma once

#include <blink/standard_parameters/speed.hpp>

namespace fudge{
namespace params {
namespace speed {

static constexpr auto UUID { blink::std_params::speed::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::speed::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter() { return blink::std_params::speed::slider_parameter(); }

} // speed
} // params
} // fudge
