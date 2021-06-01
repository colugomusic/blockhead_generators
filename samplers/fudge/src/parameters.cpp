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
	return stepify(tweak::snap_value(v, step_size, snap_amount));
}

float constrain(float v)
{
	return std::clamp(v, 0.0f, 1.0f);
};

auto increment(float v, bool precise)
{
	return constrain(stepify(tweak::increment<100, 1000>(v, precise)));
};

auto decrement(float v, bool precise)
{
	return constrain(stepify(tweak::decrement<100, 1000>(v, precise)));
};

auto drag(float v, int amount, bool precise) -> float
{
	return constrain(stepify(tweak::drag<float, 100, 1000>(v, amount / 5, precise)));
};

auto display(float v)
{
	std::stringstream ss;

	ss << convert::linear_to_ms(v) << " ms";

	return ss.str();
}

auto from_string(const std::string& str) -> std::optional<float>
{
	auto value = tweak::find_number<float>(str);

	if (!value) return std::optional<float>();

	return convert::ms_to_linear(*value);
}

} // grain_size

namespace harmonics_spread {

auto stepify(float v) -> float
{
	return tweak::std::percentage::stepify(v);
}

auto snap_value(float v, float step_size, float snap_amount)
{
	return stepify(tweak::snap_value(v, step_size, snap_amount));
}

float constrain(float v)
{
	return std::clamp(v, 0.0f, 4.0f);
};

auto increment(float v, bool precise)
{
	return constrain(stepify(tweak::increment<100, 1000>(v, precise)));
};

auto decrement(float v, bool precise)
{
	return constrain(stepify(tweak::decrement<100, 1000>(v, precise)));
};

auto drag(float v, int amount, bool precise) -> float
{
	return constrain(stepify(tweak::drag<float, 100, 1000>(v, amount / 5, precise)));
}

} // harmonics_spread

namespace sliders {

SliderSpec<float> grain_size()
{
	SliderSpec<float> out;

	out.constrain = grain_size::constrain;
	out.increment = grain_size::increment;
	out.decrement = grain_size::decrement;
	out.drag = grain_size::drag;
	out.to_string = grain_size::display;
	out.from_string = grain_size::from_string;
	out.default_value = 0.5f;

	return out;
}

SliderSpec<float> harmonic_amount()
{
	SliderSpec<float> out;

	out.constrain = [](float v) { return tweak::constrain(v, 0.0f, 4.0f); };
    out.stepify = tweak::math::stepify<100, float>;
	out.increment = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::increment<1, 10>(v, precise))); };
	out.decrement = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::decrement<1, 10>(v, precise))); };
	out.drag = [out](float v, int amount, bool precise) { return out.constrain(out.stepify(tweak::drag<float, 10, 100>(v, amount / 5, precise))); };
    out.to_string = [](float v) { return std::to_string(v); };
	out.from_string = tweak::find_positive_number<float>;
	out.default_value = 0.0f;

	return out;
}

} // sliders

namespace envelopes {

EnvelopeSpec amp()
{
	auto out = std_params::envelopes::amp();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec pan()
{
	return std_params::envelopes::pan();
}

EnvelopeSpec pitch()
{
	auto out = std_params::envelopes::pitch();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec speed()
{
	auto out = std_params::envelopes::speed();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

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
	out.range.min.to_string = grain_size::display;
	out.range.max.default_value = 1.0f;
	out.range.max.to_string = grain_size::display;
	out.to_string = grain_size::display;

	out.flags = blink_EnvelopeFlags_DefaultActive | blink_EnvelopeFlags_NoGridLabels;

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
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.uuid = "83c352fb-35b5-4cb0-a6f7-05d082b56a16";
	out.name = "Uniformity";

	return out;
}

blink::EnvelopeSpec harmonics_amount()
{
	EnvelopeSpec out;

	out.uuid = "a8a6e4fa-6713-48bb-a888-65ac437384b7";
	out.name = "Amount";

	out.get_gridline = [](int index) -> float
	{
		return float(index);
	};

	out.default_value = 0.0f;
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
    out.stepify = tweak::math::stepify<100, float>;

	out.value_slider = sliders::harmonic_amount();

	out.range.min.default_value = 0.0f;
    out.range.min.to_string = [](float v) { return std::to_string(v); };
	out.range.max.default_value = 3.0f;
    out.range.max.to_string = [](float v) { return std::to_string(v); };
    out.to_string = [](float v) { return std::to_string(v); };

	return out;
}

blink::EnvelopeSpec harmonics_spread()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.uuid = "f04c18fd-0341-4398-a02b-f3e253f658c1";
	out.name = "Spread";

	out.default_value = 1.0f;
	out.get_gridline = [](int index) { return float(index) / 4.0f; };
    out.stepify = tweak::math::stepify<100, float>;

	out.value_slider.constrain = harmonics_spread::constrain;
	out.value_slider.stepify = harmonics_spread::stepify;
	out.value_slider.decrement = harmonics_spread::decrement;
	out.value_slider.increment = harmonics_spread::increment;
	out.value_slider.drag = harmonics_spread::drag;
	out.value_slider.from_string = tweak::std::percentage::from_string;
	out.value_slider.to_string = tweak::std::percentage::to_string;

	out.range.max = out.value_slider;
	out.range.max.default_value = 2.0f;

	return out;
}

} // envelopes

namespace sliders {

blink::SliderParameterSpec<float> noise_width()
{
	auto out = std_params::sliders::parameters::noise_width();

	out.flags = blink_SliderFlags_NonGlobal;

	return out;
}

} // sliders

extern blink::ChordSpec harmonics_scale()
{
	auto out = std_params::chords::scale();

	out.flags |= blink_ChordFlags_IconOnly;
	out.flags |= blink_ChordFlags_AlwaysShowButtonWhenGroupIsVisible;

	return out;
}

} // parameters
