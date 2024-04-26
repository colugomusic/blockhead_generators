#pragma once

#include <blink/standard_parameters/amp.hpp>
#include "../parameters.h"

namespace saturator {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

inline auto envelope_parameter() {
	auto out = blink::std_params::amp::envelope_parameter();
	out.flags |= blink_ParamFlags_DefaultActive;
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Amp);
	return out;
}

inline auto slider_parameter() {
	return blink::std_params::amp::slider_parameter();
}

} // amp
} // params
} // saturator
