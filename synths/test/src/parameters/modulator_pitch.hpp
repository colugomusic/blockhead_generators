#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace test {
namespace params {
namespace modulator {
namespace pitch {

static constexpr auto UUID { "fc48a5e2-48a6-4021-8467-c196b215938d" };

auto inline envelope_parameter()
{
	auto out { blink::std_params::pitch::envelope_parameter() };

	out.uuid = UUID;
	out.name = "Modulator Pitch";
	out.short_name = "Pitch";
	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_CanManipulate;

	return out;
}

} // pitch
} // modulator
} // params
} // test
