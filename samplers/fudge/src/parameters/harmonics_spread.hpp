#pragma once

#include <blink/standard_parameters/harmonics_spread.hpp>

namespace fudge {
namespace params {
namespace harmonics_spread {

static constexpr auto UUID { blink::std_params::harmonics_spread::UUID };

auto inline envelope_parameter()
{
	return blink::std_params::harmonics_spread::envelope_parameter();
}

} // harmonics_spread
} // params
} // fudge
