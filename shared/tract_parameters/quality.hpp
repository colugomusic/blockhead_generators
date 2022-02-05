#pragma once

#include <blink/standard_parameters/search.hpp>

namespace tract_params {
namespace quality {

static constexpr auto UUID { "0111ffd7-78d1-4604-8d7b-956c34e280c3" };

inline auto envelope()
{
	blink::EnvelopeSpec out;

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
	out.searcher.binary = blink::std_params::search::float_points_binary;
	out.searcher.forward = blink::std_params::search::float_points_forward;
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

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Quality";
	out.long_desc = "When adjusted, the vocal tract model is run at a lower or higher sample rate (from 50% to 200%) which affects the perceived formant. A higher value uses more CPU.";
	out.envelope = envelope();

	out.clamp_range = { 0.0f, 2.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

inline auto envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.apply_offset = [](float value, float offset)
	{
		return value + (offset * 2);
	};

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();

	return out;
}

} // quality
} // tract_params
