#include "parameters.h"
#include "parameters/dry.hpp"
#include "parameters/feedback.hpp"
#include "parameters/time.hpp"
#include "parameters/wet.hpp"
#include "parameters/width.hpp"

using namespace blink;

namespace rattler {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.time = plugin->add_parameter(params::time::envelope_parameter());
	env.feedback = plugin->add_parameter(params::feedback::envelope_parameter());
	env.width = plugin->add_parameter(params::width::envelope_parameter());
	env.dry = plugin->add_parameter(params::dry::envelope_parameter());
	env.wet = plugin->add_parameter(params::wet::envelope_parameter());

	sliders.dry = plugin->add_parameter(params::dry::slider_parameter());
	sliders.wet = plugin->add_parameter(params::wet::slider_parameter());
	sliders.width = plugin->add_parameter(params::width::slider_parameter());
}

} // rattler
