#include "parameters.h"
#include "parameters/frequency.hpp"
#include "parameters/mix.hpp"
#include "parameters/resonance.hpp"
#include "parameters/spread.hpp"

using namespace blink;

namespace zap {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.freq = plugin->add_parameter(params::frequency::envelope_parameter());
	env.res = plugin->add_parameter(params::resonance::envelope_parameter());
	env.spread = plugin->add_parameter(params::spread::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());

	plugin->add_manipulator_target(params::frequency::UUID, params::frequency::envelope_manipulator_target());
	plugin->add_manipulator_target(params::resonance::UUID, params::resonance::envelope_manipulator_target());
	plugin->add_manipulator_target(params::spread::UUID, params::spread::envelope_manipulator_target());
	plugin->add_manipulator_target(params::mix::UUID, params::mix::envelope_manipulator_target());
}

} // zap
