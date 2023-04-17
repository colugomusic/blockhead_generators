#pragma once

#include <blink/standard_parameters/pan.hpp>
#include "../parameters.h"

namespace classic {
namespace params {
namespace pan {

static constexpr auto UUID { blink::std_params::pan::UUID };

auto inline envelope_parameter() {
	return blink::std_params::pan::envelope_parameter();
}

auto inline slider_parameter()
{
	auto out { blink::std_params::pan::slider_parameter() };
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Pan);
	return out;
}

} // pan
} // params
} // classic
