#include "parameters.h"
#include "parameters/mix.hpp"
#include "parameters/pitch.hpp"

using namespace blink;

namespace ring_modulator {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());

	plugin->add_manipulator_target(params::mix::UUID, params::mix::envelope_manipulator_target());
	plugin->add_manipulator_target(params::pitch::UUID, params::pitch::envelope_manipulator_target());
}

} // ring_modulator
