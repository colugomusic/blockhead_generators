#pragma once

#include <blink/standard_parameters/harmonics_amount.hpp>

namespace fudge {
namespace params {
namespace harmonics_amount {

static constexpr auto UUID { blink::std_params::harmonics_amount::UUID };

inline auto envelope_parameter()
{
	return blink::std_params::harmonics_amount::envelope_parameter();
}

} // harmonics_amount
} // params
} // fudge
