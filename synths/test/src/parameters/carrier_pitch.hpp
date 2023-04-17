#pragma once

#include <blink/standard_parameters/pitch.hpp>
#include "../parameters.h"

namespace test {
namespace params {
namespace carrier {
namespace pitch {

static constexpr auto UUID { blink::std_params::pitch::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::pitch::envelope_parameter() };

	out.name = "Carrier Pitch";
	out.short_name = "Pitch";
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;

	return out;
}

auto inline slider_parameter()
{
	auto out { blink::std_params::pitch::slider_parameter() };

	out.name = "Carrier Pitch";
	out.short_name = "Pitch";
	out.flags |= blink_SliderFlags_CanManipulate;
	out.manipulation_delegate = blink_Index(Parameters::Index::Env_Carrier_Pitch);

	return out;
}

} // pitch
} // carrier
} // params
} // test
