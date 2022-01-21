#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace highpass {

Parameters::Parameters(Plugin* plugin)
{
	env.freq = plugin->add_parameter(add_flags(std_params::filter_frequency::envelope_parameter(), blink_EnvelopeFlags_DefaultActive));
	env.res = plugin->add_parameter(add_flags(std_params::resonance::envelope_parameter(), blink_EnvelopeFlags_DefaultActive));
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());

	plugin->add_manipulator_target(BLINK_STD_UUID_FILTER_FREQUENCY, std_params::filter_frequency::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_FILTER_RESONANCE, std_params::resonance::envelope_manipulator_target());
	plugin->add_manipulator_target(BLINK_STD_UUID_MIX, std_params::mix::envelope_manipulator_target());
}

} // highpass
