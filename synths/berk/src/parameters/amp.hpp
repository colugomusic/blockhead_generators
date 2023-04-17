#pragma once

#include <blink/standard_parameters/amp.hpp>
#include "../parameters.h"

namespace berk {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

inline auto envelope_parameter() {
	auto out{blink::std_params::amp::envelope_parameter()};
	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	return out;
}

inline auto slider_parameter() {
	auto out{blink::std_params::amp::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Amp);
	return out;
}

} // amp
} // params
} // berk