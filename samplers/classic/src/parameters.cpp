#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace classic {

namespace envelopes {

auto amp()
{
	auto out = std_params::amp::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_AllowManipulators;

	return out;
}

auto pan()
{
	return std_params::pan::envelope_parameter();
}

auto pitch()
{
	auto out = std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_AllowManipulators;

	return out;
}

} // envelopes

namespace sliders {

auto noise_width()
{
	auto out = std_params::noise_width::slider_parameter();

	out.flags = blink_SliderFlags_NonGlobal;

	return out;
}

auto pitch()
{
	auto out = std_params::pitch::slider_parameter();

	out.flags |= blink_SliderFlags_AllowManipulators;

	return out;
}

} // sliders

Parameters::Parameters(Plugin* plugin)
{
	options.noise_mode = plugin->add_parameter(std_params::noise_mode::option());
	sliders.noise_width = plugin->add_parameter(sliders::noise_width());

	env.amp = plugin->add_parameter(envelopes::amp());
	env.pan = plugin->add_parameter(envelopes::pan());
	env.pitch = plugin->add_parameter(envelopes::pitch());

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
	sliders.pitch = plugin->add_parameter(sliders::pitch());
	sliders.sample_offset = plugin->add_parameter(std_params::sample_offset::slider_parameter());

	toggles.loop = plugin->add_parameter(std_params::loop::toggle());
	toggles.reverse = plugin->add_parameter(std_params::reverse::toggle());

	env.amp->add_manipulator_target(std_params::amp::manipulator_envelope_target());
	env.pitch->add_manipulator_target(std_params::pitch::manipulator_envelope_target());
}

} // classic
