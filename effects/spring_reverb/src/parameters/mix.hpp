#pragma once

#include <blink/standard_parameters/mix.hpp>

namespace spring_reverb {
namespace params {
namespace mix {

static constexpr auto UUID { blink::std_params::mix::UUID };

auto envelope_parameter()
{
	auto out { blink::std_params::mix::envelope_parameter() };

	out.envelope.default_value = 0.5f;

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // mix
} // params
} // spring_reverb
