#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

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

EnvelopeSpec feedback()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Feedback";
	out.uuid = "5c1ca6de-7ffd-417c-9541-2272ff1305b0";

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec damper()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Damper";
	out.uuid = "94b001e1-0796-489a-a244-e3d6fcbcfb9c";

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec pitch()
{
	auto out = std_params::envelopes::pitch();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec harmonics_amount()
{
	EnvelopeSpec out;

	out.uuid = "67180600-d95c-49d2-87cb-2d403904781c";
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
    out.range.min.to_string = [](float v) { return std::to_string(int(v)); };
	out.range.max.default_value = 3.0f;
    out.range.max.to_string = [](float v) { return std::to_string(int(v)); };
    out.to_string = [](float v) { return std::to_string(int(v)); };

	return out;
}

blink::EnvelopeSpec harmonics_spread()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.uuid = "a213c98b-a772-4db2-acd4-a5518dd48fd1";
	out.name = "Spread";

	out.default_value = 1.0f;
	out.get_gridline = [](int index) { return float(index) / 4.0f; };
    out.stepify = tweak::math::stepify<100, float>;

	out.value_slider = sliders::harmonic_spread();
	out.snap_value = [out](float v, float step_size, float snap_amount) { return out.stepify(tweak::snap_value(v, step_size, snap_amount)); };

	out.range.max = out.value_slider;
	out.range.max.default_value = 2.0f;

	return out;
}

EnvelopeSpec harmonics_scale_snap_amount()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Snap Amount";
	out.uuid = "de8c7343-360a-41fd-9788-ae2398d0a872";

	out.default_value = 1.0f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes

namespace chords {

extern blink::ChordSpec harmonics_scale()
{
	auto out = std_params::chords::scale();

	out.flags |= blink_ChordFlags_IconOnly;
	out.flags |= blink_ChordFlags_AlwaysShowButtonWhenGroupIsVisible;

	return out;
}

} // chords
} // parameters

namespace resonator {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.pitch = plugin->add_parameter(parameters::envelopes::pitch());
	env.feedback = plugin->add_parameter(parameters::envelopes::feedback());
	env.damper = plugin->add_parameter(parameters::envelopes::damper());

	const auto group_harmonics = plugin->add_group("Harmonics");
	{
		chords.harmonics_scale = plugin->add_parameter(parameters::chords::harmonics_scale());
		env.harmonics_amount = plugin->add_parameter(parameters::envelopes::harmonics_amount());
		env.harmonics_spread = plugin->add_parameter(parameters::envelopes::harmonics_spread());
		env.harmonics_scale_snap_amount = plugin->add_parameter(parameters::envelopes::harmonics_scale_snap_amount());

		chords.harmonics_scale->set_group_index(group_harmonics);
		env.harmonics_amount->set_group_index(group_harmonics);
		env.harmonics_spread->set_group_index(group_harmonics);
		env.harmonics_scale_snap_amount->set_group_index(group_harmonics);
	}

	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // resonator
