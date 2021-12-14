#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

auto SR()
{
	EnvelopeParameterSpec out;

	out.name = "Sample Rate";
	out.uuid = "1a606f6d-922f-4b5b-bd3a-a75fe291d447";

	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto BR()
{
	EnvelopeParameterSpec out;

	out.name = "Bit Reduction";
	out.uuid = "c64eeebc-65c8-44a9-a9d6-b139522df65b";

	out.envelope = std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace lofi {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.sr = plugin->add_parameter(parameters::envelopes::SR());
	env.br = plugin->add_parameter(parameters::envelopes::BR());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // lofi
