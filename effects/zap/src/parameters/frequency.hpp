#pragma once

#include <blink/standard_parameters/filter_frequency.hpp>

namespace zap {
namespace params {
namespace frequency {

static constexpr auto UUID { blink::std_params::filter_frequency::UUID };

auto envelope_parameter()
{
	auto out = blink::std_params::filter_frequency::envelope_parameter();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

} // frequency
} // params
} // zap
