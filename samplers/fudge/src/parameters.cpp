#include "parameters.h"
#include "convert.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {

namespace grain_size {

auto stepify(float v) -> float
{
	return v;
}

auto snap_value(float v, float step_size, float snap_amount)
{
	return stepify(std_params::snap_value(v, step_size, snap_amount));
}

float constrain(float v)
{
	return std::clamp(v, 0.0f, 1.0f);
};

auto increment(float v, bool precise)
{
	return constrain(stepify(std_params::increment<100, 1000>(v, precise)));
};

auto decrement(float v, bool precise)
{
	return constrain(stepify(std_params::decrement<100, 1000>(v, precise)));
};

auto drag(float v, int amount, bool precise) -> float
{
	return constrain(stepify(std_params::drag<100, 1000>(v, amount / 5, precise)));
};

auto display(float v)
{
	std::stringstream ss;

	ss << convert::linear_to_ms(v) << " ms";

	return ss.str();
}

auto from_string(const std::string& str) -> std::optional<float>
{
	auto value = std_params::find_number<float>(str);

	if (!value) return std::optional<float>();

	return convert::ms_to_linear(*value);
}

}

namespace sliders {

SliderSpec<float> grain_size()
{
	SliderSpec<float> out;

	out.constrain = grain_size::constrain;
	out.increment = grain_size::increment;
	out.decrement = grain_size::decrement;
	out.drag = grain_size::drag;
	out.display_value = grain_size::display;
	out.from_string = grain_size::from_string;
	out.default_value = 0.5f;

	return out;
}

} // sliders

namespace envelopes {

EnvelopeSpec grain_size()
{
	EnvelopeSpec out;

	out.uuid = "2a9c7ec6-68b4-40c1-aad9-b3c010cd9717";
	out.name = "Size";

	out.default_value = 0.5f;
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.stepify = grain_size::stepify;

	out.value_slider = sliders::grain_size();

	out.range.min.default_value = 0.0f;
	out.range.min.display_value = grain_size::display;
	out.range.max.default_value = 1.0f;
	out.range.max.display_value = grain_size::display;
	out.display_value = grain_size::display;

	out.flags = blink_EnvelopeFlags_NoGridLabels;

	return out;
}

EnvelopeSpec grain_transpose()
{
	EnvelopeSpec out = std_params::envelopes::pitch();

	out.uuid = "fe64baa2-9d35-4eef-bc8b-1799916e8bda";
	out.name = "Transpose";

	return out;
}

EnvelopeSpec uniformity()
{
	EnvelopeSpec out;

	out.uuid = "83c352fb-35b5-4cb0-a6f7-05d082b56a16";
	out.name = "Uniformity";

	out.default_value = 0.0f;
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.stepify = std_params::percentage::stepify;

	out.value_slider = std_params::sliders::percentage();

	out.range.min.default_value = 0.0f;
	out.range.min.display_value = std_params::percentage::display;
	out.range.max.default_value = 1.0f;
	out.range.max.display_value = std_params::percentage::display;
	out.display_value = std_params::percentage::display;

	return out;
}

} // envelopes
} // parameters