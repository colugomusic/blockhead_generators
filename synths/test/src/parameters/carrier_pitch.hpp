#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace params {
namespace carrier {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::pitch::envelope_parameter() };

	out.name = "Carrier Pitch";
	out.short_name = "Pitch";
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // pitch
} // carrier
} // params
