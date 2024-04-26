#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace lowpass {

Parameters::Parameters(Plugin* plugin)
{
	env.freq = plugin->add_parameter(add_flags(std_params::filter_frequency::envelope_parameter(), blink_ParamFlags_DefaultActive));
	env.res = plugin->add_parameter(add_flags(std_params::resonance::envelope_parameter(), blink_ParamFlags_DefaultActive));
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // lowpass
