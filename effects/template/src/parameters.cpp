#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec example()
{
	auto out = std_params::envelopes::generic::percentage();

	// https://www.uuidgenerator.net/
	out.uuid = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
	out.name = "Example Envelope";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace template {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.example = plugin->add_parameter(parameters::envelopes::example());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // template
