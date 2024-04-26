#pragma once

#include <blink/standard_parameters/wet.hpp>
#include "../parameters.h"

namespace rattler {
namespace params {
namespace wet {

static constexpr auto UUID { blink::std_params::wet::UUID };

auto inline envelope_parameter() {
	auto out { blink::std_params::wet::envelope_parameter() };
	out.flags |= blink_ParamFlags_DefaultActive; 
	return out;
}

auto inline slider_parameter() {
	auto out{blink::std_params::wet::slider_parameter()}; 
	out.slider.default_value = 1; 
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Wet);
	return out;
}

} // wet
} // params
} // rattler
