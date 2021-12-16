#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace ring_modulator {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(std_params::pitch::envelope_parameter() | blink_EnvelopeFlags_DefaultActive);
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // ring_modulator
