#pragma once

#include <blink/parameters/slider_parameter_spec.hpp>

namespace echo {
namespace params {
namespace time {

static constexpr auto UUID { "064315cd-affd-43a7-b7e8-4e586890375e" };

auto inline slider()
{
	blink::SliderSpec<float> out;

	out.constrain = [](float v)
	{
		return std::clamp(v, 0.0f, 1.0f);
	};

	out.decrement = [](float v, bool precise)
	{
		return v;
	};

	return out;
}

auto inline envelope_parameter()
{
}

auto inline slider_parameter()
{
	blink::SliderParameterSpec<float> out;

	return out;
}

auto inline envelope_manipulator_target()
{
}

} // time
} // params
} // echo
