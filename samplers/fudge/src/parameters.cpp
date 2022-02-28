#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/grain_size.hpp"
#include "parameters/grain_transpose.hpp"
#include "parameters/grain_uniformity.hpp"
#include "parameters/harmonics_amount.hpp"
#include "parameters/harmonics_scale.hpp"
#include "parameters/harmonics_spread.hpp"
#include "parameters/loop.hpp"
#include "parameters/noise_amount.hpp"
#include "parameters/noise_color.hpp"
#include "parameters/noise_mode.hpp"
#include "parameters/noise_width.hpp"
#include "parameters/pan.hpp"
#include "parameters/pitch.hpp"
#include "parameters/reverse.hpp"
#include "parameters/sample_offset.hpp"
#include "parameters/speed.hpp"

using namespace blink;

namespace fudge {

Parameters::Parameters(Plugin* plugin)
{
	options.noise_mode = plugin->add_parameter(params::noise_mode::option());
	options.reverse = plugin->add_parameter(params::reverse::option());
	sliders.noise_width = plugin->add_parameter(params::noise_width::slider_parameter());

	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
	env.speed = plugin->add_parameter(params::speed::envelope_parameter());

	auto group_geometry = plugin->add_group("Geometry");
	{
		env.grain.size = plugin->add_parameter(params::grain_size::envelope_parameter());
		env.grain.transpose = plugin->add_parameter(params::grain_transpose::envelope_parameter());
		env.grain.uniformity = plugin->add_parameter(params::grain_uniformity::envelope_parameter());

		env.grain.size->set_group_index(group_geometry);
		env.grain.transpose->set_group_index(group_geometry);
		env.grain.uniformity->set_group_index(group_geometry);
	}

	auto group_harmonics = plugin->add_group("Harmonics");
	{
		chords.scale = plugin->add_parameter(params::harmonics_scale::chord());
		env.harmonics.amount = plugin->add_parameter(params::harmonics_amount::envelope_parameter());
		env.harmonics.spread = plugin->add_parameter(params::harmonics_spread::envelope_parameter());

		chords.scale->set_group_index(group_harmonics);
		env.harmonics.amount->set_group_index(group_harmonics);
		env.harmonics.spread->set_group_index(group_harmonics);
	}

	auto group_noise = plugin->add_group("Noise");
	{
		env.noise.amount = plugin->add_parameter(params::noise_amount::envelope_parameter());
		env.noise.color = plugin->add_parameter(params::noise_color::envelope_parameter());

		env.noise.amount->set_group_index(group_noise);
		env.noise.color->set_group_index(group_noise);
		options.noise_mode->set_group_index(group_noise);
		sliders.noise_width->set_group_index(group_noise);
	}

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());
	sliders.pitch = plugin->add_parameter(params::pitch::slider_parameter());
	sliders.speed = plugin->add_parameter(params::speed::slider_parameter());
	sliders.sample_offset = plugin->add_parameter(params::sample_offset::slider_parameter());
	toggles.loop = plugin->add_parameter(params::loop::toggle());
	toggles.reverse = plugin->add_parameter(params::reverse::toggle());

	plugin->add_manipulator_target(params::amp::UUID, params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(params::pan::UUID, params::pan::envelope_manipulator_target());
	plugin->add_manipulator_target(params::pitch::UUID, params::pitch::envelope_manipulator_target());
	plugin->add_manipulator_target(params::speed::UUID, params::speed::envelope_manipulator_target());
	plugin->add_manipulator_target(params::grain_size::UUID, params::grain_size::envelope_manipulator_target());
	plugin->add_manipulator_target(params::grain_transpose::UUID, params::grain_transpose::envelope_manipulator_target());
	plugin->add_manipulator_target(params::grain_uniformity::UUID, params::grain_uniformity::envelope_manipulator_target());
	plugin->add_manipulator_target(params::harmonics_amount::UUID, params::harmonics_amount::envelope_manipulator_target());
	plugin->add_manipulator_target(params::harmonics_spread::UUID, params::harmonics_spread::envelope_manipulator_target());
	plugin->add_manipulator_target(params::noise_amount::UUID, params::noise_amount::envelope_manipulator_target());
	plugin->add_manipulator_target(params::noise_color::UUID, params::noise_color::envelope_manipulator_target());
	plugin->add_manipulator_target(params::noise_width::UUID, params::noise_width::envelope_manipulator_target());
}

} // fudge
