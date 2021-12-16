#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace test {

auto env_amp()
{
	auto out = std_params::amp::envelope_parameter();

	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto env_wave()
{
	EnvelopeParameterSpec out;

	out.uuid = "337b8f6f-1d7a-41a7-ab05-802b23887b7a";
	out.name = "wave";
	out.envelope = std_params::percentage::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto env_p0()
{
	auto out = std_params::pitch::envelope_parameter();

	out.name = "p0";
	out.uuid = "ce65f347-1891-413e-a3d4-177c6655d34c";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto env_p1()
{
	auto out = std_params::pitch::envelope_parameter();

	out.name = "p1";
	out.uuid = "fc48a5e2-48a6-4021-8467-c196b215938d";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto env_fm0()
{
	EnvelopeParameterSpec out;

	out.uuid = "05ffbc42-d2aa-4746-905e-44d4373b4345";
	out.name = "fm0";
	out.envelope = std_params::percentage::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto env_fm1()
{
	EnvelopeParameterSpec out;

	out.uuid = "343689ff-d1bf-48f0-96bf-7a6689568ef5";
	out.name = "fm1";
	out.envelope = std_params::percentage::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

Parameters::Parameters(Plugin* plugin)
{
	env.amp = plugin->add_parameter(env_amp());
	env.wave = plugin->add_parameter(env_wave());
	env.p0 = plugin->add_parameter(env_p0());
	env.p1 = plugin->add_parameter(env_p1());
	env.fm0 = plugin->add_parameter(env_fm0());
	env.fm1 = plugin->add_parameter(env_fm1());

	auto group_noise = plugin->add_group("Noise");
	{
		auto spec_sld_noise_width = std_params::noise_width::slider_parameter();

		spec_sld_noise_width.flags = blink_SliderFlags_NonGlobal;

		auto spec_env_noise_amount = std_params::noise_amount::envelope_parameter();
		auto spec_env_noise_color = std_params::noise_color::envelope_parameter();

		spec_env_noise_amount.sliders.push_back(blink_Index(Index::Sld_NoiseWidth));
		spec_env_noise_color.sliders.push_back(blink_Index(Index::Sld_NoiseWidth));
		spec_env_noise_amount.options.push_back(blink_Index(Index::Option_NoiseMode));
		spec_env_noise_color.options.push_back(blink_Index(Index::Option_NoiseMode));

		options.noise_mode = plugin->add_parameter(std_params::noise_mode::option());
		env.noise_amount = plugin->add_parameter(spec_env_noise_amount);
		env.noise_color = plugin->add_parameter(spec_env_noise_color);
		sliders.noise_width = plugin->add_parameter(spec_sld_noise_width);

		env.noise_amount->set_group_index(group_noise);
		env.noise_color->set_group_index(group_noise);
	}
}

} // test
