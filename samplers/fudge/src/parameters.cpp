#include "parameters.h"
#include "convert.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

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

} // sliders

namespace envelopes {

EnvelopeParameterSpec amp()
{
	auto out = std_params::amp::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

EnvelopeParameterSpec pan()
{
	return std_params::pan::envelope_parameter();
}

EnvelopeParameterSpec pitch()
{
	auto out = std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

EnvelopeParameterSpec speed()
{
	auto out = std_params::speed::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec grain_size()
{
	EnvelopeParameterSpec out;

	out.uuid = "2a9c7ec6-68b4-40c1-aad9-b3c010cd9717";
	out.name = "Grain Size";
	out.short_name = "Size";

	out.envelope.show_grid_labels = false;
	out.envelope.default_value = 0.5f;
	out.envelope.searcher.binary = std_params::search::float_points_binary;
	out.envelope.searcher.forward = std_params::search::float_points_forward;
	out.envelope.stepify = grain_size::stepify;

	out.envelope.value_slider = sliders::grain_size();

	out.envelope.range.min.default_value = 0.0f;
	out.envelope.range.min.to_string = grain_size::display;
	out.envelope.range.max.default_value = 1.0f;
	out.envelope.range.max.to_string = grain_size::display;
	out.envelope.to_string = grain_size::display;

	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec grain_transpose()
{
	EnvelopeParameterSpec out = std_params::pitch::envelope_parameter();

	out.uuid = "fe64baa2-9d35-4eef-bc8b-1799916e8bda";
	out.name = "Grain Transpose";
	out.short_name = "Transpose";

	return out;
}

EnvelopeParameterSpec uniformity()
{
	EnvelopeParameterSpec out;

	out.uuid = "83c352fb-35b5-4cb0-a6f7-05d082b56a16";
	out.name = "Grain Uniformity";
	out.short_name = "Uniformity";
	out.envelope = std_params::percentage::envelope();

	return out;
}

blink::EnvelopeParameterSpec harmonics_amount()
{
	EnvelopeParameterSpec out;

	out.uuid = "a8a6e4fa-6713-48bb-a888-65ac437384b7";
	out.name = "Harmonics Amount";
	out.short_name = "Amount";

	out.envelope.get_gridline = [](int index) -> float
	{
		return float(index);
	};

	out.envelope.default_value = 0.0f;
	out.envelope.searcher.binary = std_params::search::float_points_binary;
	out.envelope.searcher.forward = std_params::search::float_points_forward;
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

	out.uuid = "f04c18fd-0341-4398-a02b-f3e253f658c1";
	out.name = "Harmonics Spread";
	out.short_name = "Spread";

	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;
	out.envelope.get_gridline = [](int index) { return float(index) / 4.0f; };
    out.envelope.stepify = tweak::math::stepify<100, float>;

	out.envelope.value_slider.constrain = harmonics_spread::constrain;
	out.envelope.value_slider.stepify = harmonics_spread::stepify;
	out.envelope.value_slider.decrement = harmonics_spread::decrement;
	out.envelope.value_slider.increment = harmonics_spread::increment;
	out.envelope.value_slider.drag = harmonics_spread::drag;
	out.envelope.value_slider.from_string = tweak::std::percentage::from_string;
	out.envelope.value_slider.to_string = tweak::std::percentage::to_string;

	out.envelope.range.max = out.envelope.value_slider;
	out.envelope.range.max.default_value = 2.0f;

	return out;
}

} // envelopes

namespace sliders {

blink::SliderParameterSpec<float> noise_width()
{
	auto out = std_params::noise_width::slider_parameter();

	out.flags = blink_SliderFlags_NonGlobal;

	return out;
}

} // sliders

extern blink::ChordSpec harmonics_scale()
{
	auto out = std_params::scale::chord();

	out.flags |= blink_ChordFlags_IconOnly;
	out.flags |= blink_ChordFlags_AlwaysShowButtonWhenGroupIsVisible;

	return out;
}

namespace fudge {

Parameters::Parameters(Plugin* plugin)
{
	options.noise_mode = plugin->add_parameter(std_params::noise_mode::option());
	sliders.noise_width = plugin->add_parameter(sliders::noise_width());

	env.amp = plugin->add_parameter(envelopes::amp());
	env.pan = plugin->add_parameter(envelopes::pan());
	env.pitch = plugin->add_parameter(envelopes::pitch());
	env.speed = plugin->add_parameter(envelopes::speed());

	auto group_geometry = plugin->add_group("Geometry");
	{
		env.grain.size = plugin->add_parameter(envelopes::grain_size());
		env.grain.transpose = plugin->add_parameter(envelopes::grain_transpose());
		env.grain.uniformity = plugin->add_parameter(envelopes::uniformity());

		env.grain.size->set_group_index(group_geometry);
		env.grain.transpose->set_group_index(group_geometry);
		env.grain.uniformity->set_group_index(group_geometry);
	}

	auto group_harmonics = plugin->add_group("Harmonics");
	{
		chords.scale = plugin->add_parameter(harmonics_scale());
		env.harmonics.amount = plugin->add_parameter(envelopes::harmonics_amount());
		env.harmonics.spread = plugin->add_parameter(envelopes::harmonics_spread());

		chords.scale->set_group_index(group_harmonics);
		env.harmonics.amount->set_group_index(group_harmonics);
		env.harmonics.spread->set_group_index(group_harmonics);
	}

	auto group_noise = plugin->add_group("Noise");
	{
		auto spec_env_noise_amount = std_params::noise_amount::envelope_parameter();
		auto spec_env_noise_color = std_params::noise_color::envelope_parameter();

		spec_env_noise_amount.sliders.push_back(blink_Index(Index::Sld_NoiseWidth));
		spec_env_noise_color.sliders.push_back(blink_Index(Index::Sld_NoiseWidth));
		spec_env_noise_amount.options.push_back(blink_Index(Index::Option_NoiseMode));
		spec_env_noise_color.options.push_back(blink_Index(Index::Option_NoiseMode));

		env.noise.amount = plugin->add_parameter(spec_env_noise_amount);
		env.noise.color = plugin->add_parameter(spec_env_noise_color);

		env.noise.amount->set_group_index(group_noise);
		env.noise.color->set_group_index(group_noise);
	}

	sliders.amp = plugin->add_parameter(std_params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(std_params::pan::slider_parameter());
	sliders.pitch = plugin->add_parameter(std_params::pitch::slider_parameter());
	sliders.speed = plugin->add_parameter(std_params::speed::slider_parameter());
	sliders.sample_offset = plugin->add_parameter(std_params::sample_offset::slider_parameter());
	toggles.loop = plugin->add_parameter(std_params::loop::toggle());
	toggles.reverse = plugin->add_parameter(std_params::reverse::toggle());

	plugin->add_manipulator_target(BLINK_STD_UUID_AMP, std_params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_PITCH, std_params::pitch::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_SPEED, std_params::speed::envelope_manipulator_target());
}

} // fudge
