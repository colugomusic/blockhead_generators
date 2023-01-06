#include "parameters.h"
#include "parameters/amp.hpp"
#include "parameters/pan.hpp"

using namespace blink;

namespace gainer {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());
}

} // gainer
