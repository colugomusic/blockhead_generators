#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>
#include <blink/standard_parameters/search.hpp>
#include "../convert.h"

namespace resonator {
namespace params {
namespace fm_ratio {

static constexpr auto UUID { "25aac401-9251-4f88-af77-14dddba78151" };

struct Tweak
{
	static auto constrain(float v) { return std::clamp(v, -4.0f, 1.0f); }
	static auto decrement(float v, bool precise)
	{
		return tweak::decrement<10, 100>(v, precise);
	}
	static auto increment(float v, bool precise)
	{
		return tweak::increment<10, 100>(v, precise);
	}
	static auto drag(float v, int amount, bool precise)
	{
		return tweak::drag<float, 10, 100>(v, amount / 5, precise);
	}

	static auto stepify(float v) { return convert::ratio_to_linear(tweak::math::stepify<1000, float>(convert::linear_to_ratio(v))); }

	static auto to_string(float value) -> std::string
	{
		constexpr auto threshold = 0.001f;

		const auto milestone_hit = [threshold](float value, float milestone)
		{
			return value > milestone - threshold && value < milestone + threshold;
		};

		value = convert::linear_to_ratio(value);

		if (milestone_hit(value, 2.0f))
		{
			return "Double";
		}
		
		if (milestone_hit(value, 3.0f))
		{
			return "Triple";
		}

		if (milestone_hit(value, 0.5f))
		{
			return "Half";
		}

		if (value < 1.0f)
		{
			const auto recip = 1.0f / value;
			const auto rounded_recip = std::round(recip);

			if (std::abs(recip - rounded_recip) < threshold)
			{
				std::stringstream ss;

				ss << "1/" << rounded_recip;

				return ss.str();
			}
		}

		return tweak::to_string(value) + "x";
	}

	static auto from_string(std::string str) -> std::optional<float>
	{
		static const auto HALF = convert::ratio_to_linear(0.5f);
		static const auto DOUBLE = convert::ratio_to_linear(2.0f);
		static const auto TRIPLE = convert::ratio_to_linear(3.0f);

		std::string uppercase = str;

		std::transform(str.begin(), str.end(), uppercase.begin(), ::toupper);

		if (uppercase.find("HALF") != std::string::npos) return HALF;
		if (uppercase.find("DOUBLE") != std::string::npos) return DOUBLE;
		if (uppercase.find("TRIPLE") != std::string::npos) return TRIPLE;

		std::regex r("1/([\\d]+)");
		std::smatch match;

		if (std::regex_search(str, match, r))
		{
			return 1.0f / std::stoi(match[1]);
		}

		const auto value = tweak::find_number<float>(std::move(str));

		if (!value) return {};

		return convert::ratio_to_linear(*value);
	}
};

inline auto slider()
{
	blink::SliderSpec<float> out;

	out.constrain = Tweak::constrain;
	out.decrement = Tweak::decrement;
	out.increment = Tweak::increment;
	out.drag = Tweak::drag;
	out.from_string = Tweak::from_string;
	out.stepify = Tweak::stepify;
	out.to_string = Tweak::to_string;

	return out;
}

inline auto envelope()
{
	blink::EnvelopeSpec out;

	out.get_gridline = [](int index)
	{
		return float(index) * 0.5f;
	};

	out.value_slider = slider();
	out.value_slider.default_value = 0.0f;

	out.range.min = slider();
	out.range.min.default_value = -1.0f;
	out.range.max = slider();
	out.range.max.default_value = 1.0f;

	out.searcher.binary = blink::std_params::search::float_points_binary;
	out.searcher.forward = blink::std_params::search::float_points_forward;

	out.stepify = Tweak::stepify;
	out.to_string = Tweak::to_string;

	out.default_value = 0.0f;

	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.name = "FM Ratio";
	out.short_name = "Ratio";
	out.uuid = UUID;
	out.envelope = envelope();
	out.clamp_range = { -4.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

inline auto envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();

	return out;
}

} // fm_ratio
} // params
} // resonator