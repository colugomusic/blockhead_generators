#pragma once

#include <blink/standard_parameters/speed.hpp>
#include "../parameters.h"

namespace fudge{
namespace params {
namespace speed {

static constexpr auto UUID { blink::std_params::speed::UUID };

auto inline envelope_parameter() {
	auto out { blink::std_params::speed::envelope_parameter() };
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	return out;
}

auto inline slider_parameter() { 
	auto out{blink::std_params::speed::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Speed);
	return out;
}

} // speed
} // params
} // fudge
