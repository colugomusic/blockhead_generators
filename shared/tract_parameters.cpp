#include "tract_parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace tract_parameters {

blink::EnvelopeSpec index()
{
	auto out = std_params::envelopes::generic::percentage_bipolar();

	out.uuid = "64e246db-0d4d-4dc2-9d34-78a3a23070bc";
	out.name = "Index";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec diameter()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "ac0a8acd-29a4-42af-b75b-1d7ee2684b25";
	out.name = "Diameter";
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.default_value = 1.0f;

	out.get_gridline = [](int index) -> std::optional<float>
	{
		if (index == -1) return 0.25f;

		return std::optional<float>();
	};

	return out;
}

blink::EnvelopeSpec tongue_index()
{
	auto out = std_params::envelopes::generic::percentage_bipolar();

	out.uuid = "1059abc4-3a1c-42cc-a21b-235b2c08775b";
	out.name = "Index";
	out.default_value = 0.0f;

	return out;
}

blink::EnvelopeSpec tongue_diameter()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "a14bfe06-b84d-4778-b50c-7290478cb6b3";
	out.name = "Diameter";
	out.default_value = 0.5f;

	return out;
}

blink::EnvelopeSpec fricative_intensity()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "53c94f7f-d6b7-497e-8deb-9acd35f0d60e";
	out.name = "Intensity";
	out.default_value = 1.0f;

	return out;
}

blink::EnvelopeSpec quality()
{
	EnvelopeSpec out;

	out.uuid = "0111ffd7-78d1-4604-8d7b-956c34e280c3";
	out.name = "Quality";
	out.long_desc = "When adjusted, the vocal tract model is run at a lower or higher sample rate (from 50% to 200%) which affects the perceived formant. A higher value uses more CPU.";

	auto linear_to_quality = [](float v)
	{
		return std::pow(2.0f, v - 1.0f) * 100.0f;
	};

	auto quality_to_linear = [](float v)
	{
		return std::log(0.02f * v) / std::log(2.0f);
	};

	auto to_string = [](float v)
	{
		std::stringstream ss;

		ss << std::pow(2.0f, v - 1.0f) * 100.0f << "%";

		return ss.str();
	};

	auto from_string = [=](const ::std::string& str) -> ::std::optional<float>
	{
		auto value = tweak::find_number<float>(str);

		if (!value) return ::std::optional<float>();

		return quality_to_linear(*value);
	};

	auto stepify = [=](float v)
	{
		return quality_to_linear(tweak::math::stepify<1>(linear_to_quality(v)));
	};

	out.default_value = 1.0f;
	out.range.min.default_value = 0.0f;
	out.range.max.default_value = 2.0f;
	out.range.min.to_string = to_string;
	out.range.max.to_string = to_string;
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.stepify = stepify;
	out.to_string = to_string;

	out.value_slider.constrain = [](float v)
	{
		return std::clamp(v, 0.0f, 2.0f);
	};

	out.value_slider.increment = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::increment<1, 10>(linear_to_quality(v), precise));
	};

	out.value_slider.decrement = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::decrement<1, 10>(linear_to_quality(v), precise));
	};

	out.value_slider.drag = [=](float v, int amount, bool precise)
	{
		return quality_to_linear(tweak::drag<float, 1, 10>(linear_to_quality(v), amount / 5, precise));
	};

	out.value_slider.to_string = to_string;
	out.value_slider.from_string = from_string;
	out.value_slider.default_value = 1.0f;

	return out;
}

} // tract_parameters