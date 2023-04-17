#pragma once

#include <blink/standard_parameters/pan.hpp>
#include "../parameters.h"

namespace berk {
namespace params {
namespace pan {

static constexpr auto UUID { blink::std_params::pan::UUID };

inline auto envelope_parameter() {
	return blink::std_params::pan::envelope_parameter();
}

inline auto slider_parameter() {
	auto out{blink::std_params::pan::slider_parameter()};
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Pan);
	return out;
}

} // amp
} // params
} // berk
