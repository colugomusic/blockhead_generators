#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace classic {

namespace envelopes {

auto amp()
{
	auto out = std_params::amp::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto pan()
{
	auto out = std_params::pan::envelope_parameter();

	return out;
}

auto pitch()
{
	auto out = std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto noise_amount()
{
	auto out { std_params::noise_amount::envelope_parameter() };

	out.sliders.push_back(blink_Index(Parameters::Index::Sld_NoiseWidth));
	out.options.push_back(blink_Index(Parameters::Index::Option_NoiseMode));

	return out;
}

auto noise_color()
{
	auto out { std_params::noise_color::envelope_parameter() };

	out.sliders.push_back(blink_Index(Parameters::Index::Sld_NoiseWidth));
	out.options.push_back(blink_Index(Parameters::Index::Option_NoiseMode));

	return out;
}

} // envelopes

namespace sliders {

auto noise_width()
{
	auto out { std_params::noise_width::slider_parameter() };

	out.flags = blink_SliderFlags_NonGlobal;

	return out;
}

auto pitch()
{
	auto out { std_params::pitch::slider_parameter() };

	out.flags |= blink_SliderFlags_CanManipulate;

	return out;
}

} // sliders

namespace toggles {

auto reverse()
{
	auto out { std_params::reverse::toggle() };

	out.flags |= blink_ToggleFlags_CanManipulate;
	out.flags |= blink_ToggleFlags_IsManipulatorTarget;

	return out;
}

} // toggles

Parameters::Parameters(Plugin* plugin)
{
	options.noise_mode = plugin->add_parameter(std_params::noise_mode::option());
	sliders.noise_width = plugin->add_parameter(sliders::noise_width());

	env.amp = plugin->add_parameter(envelopes::amp());
	env.pan = plugin->add_parameter(envelopes::pan());
	env.pitch = plugin->add_parameter(envelopes::pitch());

	auto group_noise = plugin->add_group("Noise");
	{
		env.noise.amount = plugin->add_parameter(envelopes::noise_amount());
		env.noise.color = plugin->add_parameter(envelopes::noise_color());

		env.noise.amount->set_group_index(group_noise);
		env.noise.color->set_group_index(group_noise);
	}

	sliders.amp = plugin->add_parameter(std_params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(std_params::pan::slider_parameter());
	sliders.pitch = plugin->add_parameter(sliders::pitch());
	sliders.sample_offset = plugin->add_parameter(std_params::sample_offset::slider_parameter());

	toggles.loop = plugin->add_parameter(std_params::loop::toggle());
	toggles.reverse = plugin->add_parameter(toggles::reverse());

	plugin->add_manipulator_target(BLINK_STD_UUID_AMP, std_params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_PAN, std_params::pan::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_PITCH, std_params::pitch::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_NOISE_AMOUNT, std_params::noise_amount::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_NOISE_COLOR, std_params::noise_color::envelope_manipulator_target());
}

} // classic
