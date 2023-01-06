#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/drive.hpp"
#include "parameters/mix.hpp"

using namespace blink;

namespace saturator {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.drive = plugin->add_parameter(params::drive::envelope_parameter());
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());
}

} // saturator
