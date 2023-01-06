#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace berk {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

inline auto envelope_parameter()
{
	auto out { blink::std_params::amp::envelope_parameter() };

	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

inline auto slider_parameter() { return blink::std_params::amp::slider_parameter(); }

} // amp
} // params
} // berk