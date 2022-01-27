#pragma once

#include <blink/standard_parameters/noise_color.hpp>
#include "../parameters.h"

namespace fudge {
namespace params {
namespace noise_color {

static constexpr auto UUID { blink::std_params::noise_color::UUID };

auto inline envelope_parameter()
{
	auto out { blink::std_params::noise_color::envelope_parameter() };

	out.sliders.push_back(blink_Index(Parameters::Index::Sld_NoiseWidth));
	out.options.push_back(blink_Index(Parameters::Index::Option_NoiseMode));

	return out;
}

auto inline envelope_manipulator_target() { return blink::std_params::noise_color::envelope_manipulator_target(); }

} // noise_color
} // params
} // fudge
