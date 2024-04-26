#pragma once

#include <blink/standard_parameters/pitch.hpp>
#include "../parameters.h"

namespace berk {
namespace params {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

inline auto envelope_parameter() {
	auto out{blink::std_params::pitch::envelope_parameter()};
	out.flags |= blink_ParamFlags_DefaultActive;
	return out;
}

inline auto slider_parameter() {
	auto out{blink::std_params::pitch::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Pitch);
	return out;
}

} // pitch
} // params
} // berk
