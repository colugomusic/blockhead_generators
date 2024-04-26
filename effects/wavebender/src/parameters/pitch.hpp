#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace wavebender {
namespace params {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

auto envelope_parameter()
{
	auto out = blink::std_params::pitch::envelope_parameter();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

} // pitch
} // params
} // wavebender
