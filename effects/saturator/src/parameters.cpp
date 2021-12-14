#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

auto drive()
{
	EnvelopeParameterSpec out;

	out.name = "Drive";
	out.uuid = "ad7727c7-f73c-49ff-919d-9a48a75995a4";
	out.envelope = std_params::percentage::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto amp()
{
	auto out = std_params::amp::envelope_parameter();

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
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // saturator
