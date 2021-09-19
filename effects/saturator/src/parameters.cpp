#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec drive()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Drive";
	out.uuid = "ad7727c7-f73c-49ff-919d-9a48a75995a4";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec amp()
{
	auto out = std_params::envelopes::amp();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace saturator {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.drive = plugin->add_parameter(parameters::envelopes::drive());
	env.amp = plugin->add_parameter(parameters::envelopes::amp());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // saturator
