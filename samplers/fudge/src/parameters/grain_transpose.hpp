#pragma once

#include <blink/standard_parameters/pitch.hpp>

namespace fudge {
namespace params {
namespace grain_transpose {

static constexpr auto UUID { "fe64baa2-9d35-4eef-bc8b-1799916e8bda" };

auto inline envelope_parameter()
{
	auto out { blink::std_params::pitch::envelope_parameter() };

	out.uuid = UUID;
	out.name = "Grain Transpose";
	out.short_name = "Transpose";

	return out;
}

} // grain_transpose
} // params
} // fudge
