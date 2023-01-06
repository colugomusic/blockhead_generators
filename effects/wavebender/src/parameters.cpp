#include "parameters.h"
#include "parameters/bubble.hpp"
#include "parameters/mix.hpp"
#include "parameters/pitch.hpp"
#include "parameters/smoother.hpp"
#include "parameters/tilt.hpp"
#include "parameters/xfade_mode.hpp"
#include "parameters/xfade_size.hpp"
#include <blink/standard_parameters/all.hpp>
#include "plugin.h"

using namespace blink;

namespace wavebender {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.tilt = plugin->add_parameter(params::tilt::envelope_parameter());
	env.xfade_size = plugin->add_parameter(params::xfade_size::envelope_parameter());
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
	env.bubble = plugin->add_parameter(params::bubble::envelope_parameter());
	env.smoother = plugin->add_parameter(params::smoother::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());
	opt.xfade_mode = plugin->add_parameter(params::xfade_mode::option());
}

} // wavebender