#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace freeze {

Parameters::Parameters(Plugin* plugin)
{
	env.pitch = plugin->add_parameter(std_params::pitch::envelope_parameter());
	env.formant = plugin->add_parameter(std_params::formant::envelope_parameter());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
	sliders.pitch = plugin->add_parameter(std_params::pitch::slider_parameter());
}

} // freeze
