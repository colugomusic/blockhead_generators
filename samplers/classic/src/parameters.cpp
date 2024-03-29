#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/loop.hpp"
#include "parameters/noise_amount.hpp"
#include "parameters/noise_color.hpp"
#include "parameters/noise_mode.hpp"
#include "parameters/noise_width.hpp"
#include "parameters/pan.hpp"
#include "parameters/pitch.hpp"
#include "parameters/reverse.hpp"
#include "parameters/sample_offset.hpp"

using namespace blink;

namespace classic {

Parameters::Parameters(Plugin* plugin)
{
	options.noise_mode = plugin->add_parameter(params::noise_mode::option());
	options.reverse = plugin->add_parameter(params::reverse::option());
	sliders.noise_width = plugin->add_parameter(params::noise_width::slider_parameter());

	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());

	auto group_noise = "Noise";
	{
		env.noise.amount = plugin->add_parameter(params::noise_amount::envelope_parameter());
		env.noise.color = plugin->add_parameter(params::noise_color::envelope_parameter());

		env.noise.amount->set_group_name(group_noise);
		env.noise.color->set_group_name(group_noise);
		options.noise_mode->set_group_name(group_noise);
		sliders.noise_width->set_group_name(group_noise);
	}

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());
	sliders.pitch = plugin->add_parameter(params::pitch::slider_parameter());
	sliders.sample_offset = plugin->add_parameter(params::sample_offset::slider_parameter());

	toggles.loop = plugin->add_parameter(params::loop::toggle());
	toggles.reverse = plugin->add_parameter(params::reverse::toggle());
}

} // classic
