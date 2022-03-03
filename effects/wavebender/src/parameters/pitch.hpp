#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace wavebender {
namespace params {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

auto envelope_parameter()
{
	auto out = blink::std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline envelope_manipulator_target() { return blink::std_params::pitch::envelope_manipulator_target(); }

} // pitch
} // params
} // wavebender
