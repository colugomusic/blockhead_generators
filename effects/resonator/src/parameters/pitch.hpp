#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace resonator {
namespace params {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

inline auto slider_parameter() { return blink::std_params::pitch::slider_parameter(); }

inline auto envelope_parameter()
{
	auto out = blink::std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // pitch
} // params
} // resonator
