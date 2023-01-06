#include "parameters.h"
#include "parameters/mix.hpp"
#include "parameters/pitch.hpp"

using namespace blink;

namespace ring_modulator {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());
}

} // ring_modulator
