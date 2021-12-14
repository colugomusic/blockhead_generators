#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeParameterSpec frequency()
{
	auto out = std_params::filter_frequency::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec resonance()
{
	auto out = std_params::resonance::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec spread()
{
	EnvelopeParameterSpec out;

	out.name = "Spread";
	out.uuid = "7112fe4a-7d61-4b48-acd3-0c8f8fa34fa5";
	out.envelope = std_params::percentage::bipolar::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace zap {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.freq = plugin->add_parameter(parameters::envelopes::frequency());
	env.res = plugin->add_parameter(parameters::envelopes::resonance());
	env.spread = plugin->add_parameter(parameters::envelopes::spread());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // zap
