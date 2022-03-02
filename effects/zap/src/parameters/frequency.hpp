#pragma once

#include <blink/standard_parameters/filter_frequency.hpp>

namespace zap {
namespace params {
namespace frequency {

static constexpr auto UUID { blink::std_params::filter_frequency::UUID };

auto envelope_parameter()
{
	auto out = blink::std_params::filter_frequency::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline envelope_manipulator_target()
{
	return blink::std_params::filter_frequency::envelope_manipulator_target();
}

} // frequency
} // params
} // zap
