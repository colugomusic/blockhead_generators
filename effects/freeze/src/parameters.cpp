#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace freeze {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(add_flags(std_params::pitch::envelope_parameter(), blink_EnvelopeFlags_DefaultActive));
	env.formant = plugin->add_parameter(add_flags(std_params::formant::envelope_parameter(), blink_EnvelopeFlags_DefaultActive));
	env.mix = plugin->add_parameter(add_flags(std_params::mix::envelope_parameter(), blink_EnvelopeFlags_DefaultActive));
	sliders.pitch = plugin->add_parameter(std_params::pitch::slider_parameter());
}

} // freeze
