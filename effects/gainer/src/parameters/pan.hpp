#pragma once

#include <blink/standard_parameters/pan.hpp>
#include "../parameters.h"

namespace gainer {
namespace params {
namespace pan {

static constexpr auto UUID { blink::std_params::pan::UUID };

auto inline envelope_parameter() {
	auto out { blink::std_params::pan::envelope_parameter() };
	out.flags |= blink_ParamFlags_DefaultActive;
	return out;
}

auto inline slider_parameter() {
	auto out{blink::std_params::pan::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Pan);
	return out;
}

} // pan
} // params
} // gainer
