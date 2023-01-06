#pragma once

#include <blink/standard_parameters/search.hpp>

namespace compressor {
namespace params {
namespace ratio {

static constexpr auto UUID { "29bf39a4-b3c4-4a48-b577-dc252ce725c5" };

inline auto envelope()
{
	blink::EnvelopeSpec out;

	out.default_value = tweak::convert::ratio_to_linear(4.0f);
	out.get_gridline = [](int index) -> std::optional<float>
	{
		switch (index)
		{
			case 1: return tweak::convert::ratio_to_linear(1.0f);
			case 2: return tweak::convert::ratio_to_linear(2.0f);
			case 3: return tweak::convert::ratio_to_linear(4.0f);
			case 4: return tweak::convert::ratio_to_linear(8.0f);
			case 5: return tweak::convert::ratio_to_linear(16.0f);
			case 6: return tweak::convert::ratio_to_linear(32.0f);
			case 7: return tweak::convert::ratio_to_linear(64.0f);

			default: return std::optional<float>();
		}
	};
	out.searcher.binary = blink::std_params::search::float_points_binary;
	out.searcher.forward = blink::std_params::search::float_points_forward;
	out.stepify = [](float v)
	{
		return tweak::convert::ratio_to_linear(tweak::math::stepify<1000>(tweak::convert::linear_to_ratio(v)));
	};
	out.to_string = [](float v)
	{
		std::stringstream ss;

		ss << tweak::convert::linear_to_ratio(v) << " : 1";

		return ss.str();
	};

	out.range.min.default_value = 0.0f;
	out.range.min.to_string = out.to_string;

	out.range.max.default_value = 1.0f;
	out.range.max.to_string = out.to_string;

	out.value_slider.constrain = [](float v)
	{
		if (v < 0) return 0.0f;
		if (v > 1) return 1.0f;
		return v;
	};

	out.value_slider.decrement = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::decrement<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.value_slider.increment = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::increment<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.value_slider.default_value = out.default_value;

	out.value_slider.drag = [](float value, int amount, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::drag<float, 10, 100>(tweak::convert::linear_to_ratio(value), amount / 5, precise));
	};

	out.value_slider.from_string = [](std::string str) -> std::optional<float>
	{
		const auto number = tweak::find_positive_number<float>(std::move(str));

		if (!number) return number;

		return tweak::convert::ratio_to_linear(*number);
	};

	out.value_slider.stepify = out.stepify;
	out.value_slider.to_string = out.to_string;

	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Ratio";
	out.envelope = envelope();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	out.override_envelope = envelope();

	return out;
}

} // ratio
} // params
} // compressor
