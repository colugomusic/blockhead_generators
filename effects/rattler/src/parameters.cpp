#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/feedback.hpp"
#include "parameters/time.hpp"
#include "parameters/width.hpp"

using namespace blink;

namespace rattler {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.time = plugin->add_parameter(params::time::envelope_parameter());
	env.feedback = plugin->add_parameter(params::feedback::envelope_parameter());
	env.width = plugin->add_parameter(params::width::envelope_parameter());

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.width = plugin->add_parameter(params::width::slider_parameter());

	plugin->add_manipulator_target(params::amp::UUID, params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(params::time::UUID, params::time::envelope_manipulator_target());
	plugin->add_manipulator_target(params::feedback::UUID, params::feedback::envelope_manipulator_target());
	plugin->add_manipulator_target(params::width::UUID, params::width::envelope_manipulator_target());
}

} // rattler
