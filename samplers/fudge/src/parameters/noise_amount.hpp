#pragma once

#include <blink/standard_parameters/noise_amount.hpp>
#include "../parameters.h"

namespace fudge {
namespace params {
namespace noise_amount {

static constexpr auto UUID { blink::std_params::noise_amount::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::noise_amount::envelope_parameter() };

	out.sliders.push_back(blink_Index(Parameters::Index::Sld_NoiseWidth));
	out.options.push_back(blink_Index(Parameters::Index::Option_NoiseMode));

	return out;
}

auto inline envelope_manipulator_target() { return blink::std_params::noise_amount::envelope_manipulator_target(); }

} // noise_amount
} // params
} // fudge
