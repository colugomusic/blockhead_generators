#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec dispersion()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Dispersion";
	out.uuid = "5c1ca6de-7ffd-417c-9541-2272ff1305b0";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec pitch()
{
	auto out = std_params::envelopes::pitch();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace resonator {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.pitch = plugin->add_parameter(parameters::envelopes::pitch());
	env.dispersion = plugin->add_parameter(parameters::envelopes::dispersion());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // resonator
