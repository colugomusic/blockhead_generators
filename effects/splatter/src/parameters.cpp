#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec example()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Example Envelope";
	out.uuid = "4bbded86-953f-4a0d-98e6-99a5b7f6c7c2";

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace splatter {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.example = plugin->add_parameter(parameters::envelopes::example());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // splatter
