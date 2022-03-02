#include "parameters.h"
#include "parameters/decay.hpp"
#include "parameters/size.hpp"
#include "parameters/mix.hpp"

using namespace blink;

namespace spring_reverb {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.size = plugin->add_parameter(params::size::envelope_parameter());
	env.decay = plugin->add_parameter(params::decay::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());

	plugin->add_manipulator_target(params::decay::UUID, params::decay::envelope_manipulator_target());
	plugin->add_manipulator_target(params::mix::UUID, params::mix::envelope_manipulator_target());
	plugin->add_manipulator_target(params::size::UUID, params::size::envelope_manipulator_target());
}

} // spring_reverb