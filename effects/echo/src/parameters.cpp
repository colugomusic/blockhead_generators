#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/feedback.hpp"
#include "parameters/time.hpp"

using namespace blink;

namespace echo {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());

	plugin->add_manipulator_target(params::amp::UUID, params::amp::envelope_manipulator_target());
	plugin->add_manipulator_target(params::pan::UUID, params::pan::envelope_manipulator_target());
}

} // echo
