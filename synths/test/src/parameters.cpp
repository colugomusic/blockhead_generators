#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/carrier_fm.hpp"
#include "parameters/carrier_pitch.hpp"
#include "parameters/carrier_waveform.hpp"
#include "parameters/modulator_fm.hpp"
#include "parameters/modulator_pitch.hpp"
#include "parameters/pan.hpp"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace test {

Parameters::Parameters(Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());

	auto group_carrier = "Carrier";
	{
		env.carrier.wave = plugin->add_parameter(params::carrier::waveform::envelope_parameter());
		env.carrier.pitch = plugin->add_parameter(params::carrier::pitch::envelope_parameter());
		env.carrier.fm = plugin->add_parameter(params::carrier::fm::envelope_parameter());

		env.carrier.wave->set_group_name(group_carrier);
		env.carrier.pitch->set_group_name(group_carrier);
		env.carrier.fm->set_group_name(group_carrier);
	}

	auto group_modulator = "Modulator";
	{
		env.modulator.pitch = plugin->add_parameter(params::modulator::pitch::envelope_parameter());
		env.modulator.fm = plugin->add_parameter(params::modulator::fm::envelope_parameter());

		env.modulator.pitch->set_group_name(group_modulator);
		env.modulator.fm->set_group_name(group_modulator);
	}

	auto group_noise = "Noise";
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

		env.noise_amount->set_group_name(group_noise);
		env.noise_color->set_group_name(group_noise);
		options.noise_mode->set_group_name(group_noise);
		sliders.noise_width->set_group_name(group_noise);
	}

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());
	sliders.carrier_pitch = plugin->add_parameter(params::carrier::pitch::slider_parameter());
}

} // test
