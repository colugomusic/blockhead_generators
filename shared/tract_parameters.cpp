#include "tract_parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace tract_parameters {

EnvelopeParameterSpec index()
{
	EnvelopeParameterSpec out;

	out.uuid = "64e246db-0d4d-4dc2-9d34-78a3a23070bc";
	out.name = "Index";
	out.envelope = std_params::percentage::bipolar::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec diameter()
{
	EnvelopeParameterSpec out;

	out.uuid = "ac0a8acd-29a4-42af-b75b-1d7ee2684b25";
	out.name = "Diameter";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;
	out.envelope.get_gridline = [](int index) -> std::optional<float>
	{
		if (index == -1) return 0.25f;

		return std::optional<float>();
	};

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec tongue_index()
{
	EnvelopeParameterSpec out;

	out.uuid = "1059abc4-3a1c-42cc-a21b-235b2c08775b";
	out.name = "Index";
	out.envelope = std_params::percentage::bipolar::envelope();
	out.envelope.default_value = 0.0f;

	return out;
}

EnvelopeParameterSpec tongue_diameter()
{
	EnvelopeParameterSpec out;

	out.uuid = "a14bfe06-b84d-4778-b50c-7290478cb6b3";
	out.name = "Diameter";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;

	return out;
}

EnvelopeParameterSpec fricative_intensity()
{
	EnvelopeParameterSpec out;

	out.uuid = "53c94f7f-d6b7-497e-8deb-9acd35f0d60e";
	out.name = "Intensity";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;

	return out;
}

EnvelopeParameterSpec quality()
{
	EnvelopeParameterSpec out;

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

	out.envelope.default_value = 1.0f;
	out.envelope.range.min.default_value = 0.0f;
	out.envelope.range.max.default_value = 2.0f;
	out.envelope.range.min.to_string = to_string;
	out.envelope.range.max.to_string = to_string;
	out.envelope.search_binary = std_params::search::envelope_binary;
	out.envelope.search_forward = std_params::search::envelope_forward;
	out.envelope.stepify = stepify;
	out.envelope.to_string = to_string;

	out.envelope.value_slider.constrain = [](float v)
	{
		return std::clamp(v, 0.0f, 2.0f);
	};

	out.envelope.value_slider.increment = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::increment<1, 10>(linear_to_quality(v), precise));
	};

	out.envelope.value_slider.decrement = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::decrement<1, 10>(linear_to_quality(v), precise));
	};

	out.envelope.value_slider.drag = [=](float v, int amount, bool precise)
	{
		return quality_to_linear(tweak::drag<float, 1, 10>(linear_to_quality(v), amount / 5, precise));
	};

	out.envelope.value_slider.to_string = to_string;
	out.envelope.value_slider.from_string = from_string;
	out.envelope.value_slider.default_value = 1.0f;

	return out;
}

} // tract_parameters