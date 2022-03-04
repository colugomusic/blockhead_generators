#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace berk {
namespace params {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

inline auto envelope_parameter()
{
	auto out { blink::std_params::pitch::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

inline auto envelope_manipulator_target()
{
	return blink::std_params::pitch::envelope_manipulator_target();
}

} // pitch
} // params
} // berk
