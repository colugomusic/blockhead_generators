#include "parameters.h"
#include "convert.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace resonator {
namespace parameters {
namespace sliders {

SliderSpec<float> harmonic_amount()
{
	SliderSpec<float> out;

	out.constrain = [](float v) { return tweak::constrain(v, 0.0f, 3.0f); };
    out.stepify = tweak::math::stepify<100, float>;
	out.increment = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::increment<1, 10>(v, precise))); };
	out.decrement = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::decrement<1, 10>(v, precise))); };
	out.drag = [out](float v, int amount, bool precise) { return out.constrain(out.stepify(tweak::drag<float, 10, 100>(v, amount / 5, precise))); };
    out.to_string = [](float v) { return tweak::to_string(v); };
	out.from_string = tweak::find_positive_number<float>;
	out.default_value = 0.0f;

	return out;
}

SliderSpec<float> harmonic_spread()
{
	SliderSpec<float> out;

	out.constrain = [](float v) { return tweak::constrain(v, 0.0f, 4.0f); };
    out.stepify = tweak::std::percentage::stepify;
	out.increment = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::increment<100, 1000>(v, precise))); };
	out.decrement = [out](float v, bool precise) { return out.constrain(out.stepify(tweak::decrement<100, 1000>(v, precise))); };
	out.drag = [out](float v, int amount, bool precise) { return out.constrain(out.stepify(tweak::drag<float, 100, 1000>(v, amount / 5, precise))); };
    out.to_string = [](float v) { return tweak::to_string(v); };
	out.from_string = tweak::std::percentage::from_string;
	out.default_value = 0.0f;

	return out;
}

} // sliders

namespace envelopes {

EnvelopeParameterSpec feedback()
{
	EnvelopeParameterSpec out;

	out.name = "Feedback";
	out.uuid = "5c1ca6de-7ffd-417c-9541-2272ff1305b0";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec damper()
{
	EnvelopeParameterSpec out;

	out.name = "Damper";
	out.uuid = "94b001e1-0796-489a-a244-e3d6fcbcfb9c";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec pitch()
{
	auto out = std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec fm_amount()
{
	EnvelopeParameterSpec out;

	out.name = "FM Amount";
	out.short_name = "Amount";
	out.uuid = "d1b4d493-35f9-408b-982c-4474517a968c";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 0.0f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec fm_ratio()
{
	EnvelopeParameterSpec out;

	out.name = "FM Ratio";
	out.short_name = "Ratio";
	out.uuid = "25aac401-9251-4f88-af77-14dddba78151";

	struct Tweak
	{
		static auto constrain(float v) { return std::clamp(v, -4.0f, 1.0f); }
		static auto decrement(float v, bool precise)
		{
			return tweak::decrement<10, 100>(v, precise);
			//return Convert::linear_to_ratio(tweak::decrement<10, 100>(Convert::ratio_to_linear(v), precise));
		}
		static auto increment(float v, bool precise)
		{
			return tweak::increment<10, 100>(v, precise);
			//return Convert::linear_to_ratio(tweak::increment<10, 100>(Convert::ratio_to_linear(v), precise));
		}
		static auto drag(float v, int amount, bool precise)
		{
			return tweak::drag<float, 10, 100>(v, amount / 5, precise);
			//return Convert::linear_to_ratio(tweak::drag<float, 10, 100>(Convert::ratio_to_linear(v), amount / 5, precise));
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

		static auto from_string(const std::string& str) -> std::optional<float>
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

			const auto value = tweak::find_number<float>(str);

			if (!value) return {};

			return convert::ratio_to_linear(*value);
		}
	};

	out.envelope.get_gridline = [](int index)
	{
		return float(index) * 0.5f;
	};

	SliderSpec<float> slider_spec;

	slider_spec.constrain = Tweak::constrain;
	slider_spec.decrement = Tweak::decrement;
	slider_spec.increment = Tweak::increment;
	slider_spec.drag = Tweak::drag;
	slider_spec.from_string = Tweak::from_string;
	slider_spec.stepify = Tweak::stepify;
	slider_spec.to_string = Tweak::to_string;

	out.envelope.value_slider = slider_spec;
	out.envelope.value_slider.default_value = 0.0f;

	out.envelope.range.min = slider_spec;
	out.envelope.range.min.default_value = -1.0f;
	out.envelope.range.max = slider_spec;
	out.envelope.range.max.default_value = 1.0f;

	out.envelope.search_binary = std_params::search::envelope_binary;
	out.envelope.search_forward = std_params::search::envelope_forward;

	out.envelope.stepify = Tweak::stepify;
	out.envelope.to_string = Tweak::to_string;

	out.envelope.default_value = 0.0f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec harmonics_amount()
{
	EnvelopeParameterSpec out;

	out.uuid = "67180600-d95c-49d2-87cb-2d403904781c";
	out.name = "Harmonics Amount";
	out.short_name = "Amount";

	out.envelope.get_gridline = [](int index) -> float
	{
		return float(index);
	};

	out.envelope.default_value = 0.0f;
	out.envelope.search_binary = std_params::search::envelope_binary;
	out.envelope.search_forward = std_params::search::envelope_forward;
    out.envelope.stepify = tweak::math::stepify<100, float>;

	out.envelope.value_slider = sliders::harmonic_amount();

	out.envelope.range.min.default_value = 0.0f;
    out.envelope.range.min.to_string = [](float v) { return std::to_string(int(v)); };
	out.envelope.range.max.default_value = 3.0f;
    out.envelope.range.max.to_string = [](float v) { return std::to_string(int(v)); };
    out.envelope.to_string = [](float v) { return std::to_string(int(v)); };

	return out;
}

EnvelopeParameterSpec harmonics_spread()
{
	EnvelopeParameterSpec out;

	out.uuid = "a213c98b-a772-4db2-acd4-a5518dd48fd1";
	out.name = "Harmonics Spread";
	out.short_name = "Spread";

	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;
	out.envelope.get_gridline = [](int index) { return float(index) / 4.0f; };
    out.envelope.stepify = tweak::math::stepify<100, float>;

	out.envelope.value_slider = sliders::harmonic_spread();
	out.envelope.snap_value = [out](float v, float step_size, float snap_amount) { return out.envelope.stepify(tweak::snap_value(v, step_size, snap_amount)); };

	out.envelope.range.max = out.envelope.value_slider;
	out.envelope.range.max.default_value = 2.0f;

	return out;
}

EnvelopeParameterSpec harmonics_scale_snap_amount()
{
	EnvelopeParameterSpec out;

	out.name = "Harmonics Scale Snap Amount";
	out.short_name = "Snap Amount";
	out.long_desc = "Controls the amount of scale snapping for the harmonics";
	out.uuid = "de8c7343-360a-41fd-9788-ae2398d0a872";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;

	return out;
}

EnvelopeParameterSpec harmonics_width()
{
	EnvelopeParameterSpec out;

	out.name = "Harmonics Width";
	out.short_name = "Width";
	out.uuid = "2f95dc68-9ae3-4e12-8ef1-81c36201b995";
	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;

	return out;
}

} // envelopes

namespace chords {

extern ChordSpec harmonics_scale()
{
	auto out = std_params::scale::chord();

	out.flags |= blink_ChordFlags_IconOnly;
	out.flags |= blink_ChordFlags_AlwaysShowButtonWhenGroupIsVisible;

	return out;
}

} // chords
} // parameters

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(parameters::envelopes::pitch());
	env.feedback = plugin->add_parameter(parameters::envelopes::feedback());
	env.damper = plugin->add_parameter(parameters::envelopes::damper());

	const auto group_fm = plugin->add_group("FM");
	{
		env.fm.amount = plugin->add_parameter(parameters::envelopes::fm_amount());
		env.fm.ratio = plugin->add_parameter(parameters::envelopes::fm_ratio());

		env.fm.amount->set_group_index(group_fm);
		env.fm.ratio->set_group_index(group_fm);
	}

	const auto group_harmonics = plugin->add_group("Harmonics");
	{
		chords.harmonics.scale = plugin->add_parameter(parameters::chords::harmonics_scale());
		env.harmonics.amount = plugin->add_parameter(parameters::envelopes::harmonics_amount());
		env.harmonics.spread = plugin->add_parameter(parameters::envelopes::harmonics_spread());
		env.harmonics.scale_snap_amount = plugin->add_parameter(parameters::envelopes::harmonics_scale_snap_amount());
		env.harmonics.width = plugin->add_parameter(parameters::envelopes::harmonics_width());

		chords.harmonics.scale->set_group_index(group_harmonics);
		env.harmonics.amount->set_group_index(group_harmonics);
		env.harmonics.spread->set_group_index(group_harmonics);
		env.harmonics.scale_snap_amount->set_group_index(group_harmonics);
	}

	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());

	sliders.pitch = plugin->add_parameter(std_params::pitch::slider_parameter());
}

} // resonator
