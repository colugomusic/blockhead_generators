#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec frequency()
{
	auto out = std_params::envelopes::filter_frequency();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec resonance()
{
	auto out = std_params::envelopes::resonance();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec spread()
{
	auto out = std_params::envelopes::generic::percentage_bipolar();

	out.name = "Spread";
	out.uuid = "7112fe4a-7d61-4b48-acd3-0c8f8fa34fa5";

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
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // zap
