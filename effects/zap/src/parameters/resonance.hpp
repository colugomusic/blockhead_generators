#pragma once

#include <blink/standard_parameters/resonance.hpp>

namespace zap {
namespace params {
namespace resonance {

static constexpr auto UUID { blink::std_params::resonance::UUID };

auto envelope_parameter()
{
	auto out { blink::std_params::resonance::envelope_parameter() };

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline envelope_manipulator_target()
{
	return blink::std_params::resonance::envelope_manipulator_target();
}

} // resonance
} // params
} // zap
