#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace ring_modulator {
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

auto inline envelope_manipulator_target()
{
	return blink::std_params::pitch::envelope_manipulator_target();
}

} // pitch
} // params
} // ring_modulator
