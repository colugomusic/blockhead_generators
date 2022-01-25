#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/carrier_fm.hpp"
#include "parameters/carrier_pitch.hpp"
#include "parameters/carrier_waveform.hpp"
#include "parameters/modulator_fm.hpp"
#include "parameters/modulator_pitch.hpp"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace test {

Parameters::Parameters(Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());

	auto group_carrier = plugin->add_group("Carrier");
	{
		env.carrier.wave = plugin->add_parameter(params::carrier::waveform::envelope_parameter());
		env.carrier.pitch = plugin->add_parameter(params::carrier::pitch::envelope_parameter());
		env.carrier.fm = plugin->add_parameter(params::carrier::fm::envelope_parameter());

		env.carrier.wave->set_group_index(group_carrier);
		env.carrier.pitch->set_group_index(group_carrier);
		env.carrier.fm->set_group_index(group_carrier);
	}

	auto group_modulator = plugin->add_group("Modulator");
	{
		env.modulator.pitch = plugin->add_parameter(params::modulator::pitch::envelope_parameter());
		env.modulator.fm = plugin->add_parameter(params::modulator::fm::envelope_parameter());

		env.modulator.pitch->set_group_index(group_modulator);
		env.modulator.fm->set_group_index(group_modulator);
	}

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
		sliders.noise_width->set_group_index(group_noise);
	}

	plugin->add_manipulator_target(params::amp::UUID, std_params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(params::carrier::waveform::UUID, params::carrier::waveform::envelope_manipulator_target());
	plugin->add_manipulator_target(params::carrier::pitch::UUID, std_params::pitch::envelope_manipulator_target());
	plugin->add_manipulator_target(params::carrier::fm::UUID, params::carrier::fm::envelope_manipulator_target());
	plugin->add_manipulator_target(params::modulator::pitch::UUID, std_params::pitch::envelope_manipulator_target());
	plugin->add_manipulator_target(params::modulator::fm::UUID, params::modulator::fm::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_NOISE_AMOUNT, std_params::noise_amount::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_NOISE_COLOR, std_params::noise_color::envelope_manipulator_target());
}

} // test
