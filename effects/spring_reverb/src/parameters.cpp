#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace spring_reverb {
namespace parameters {
namespace envelopes {

blink::EnvelopeSpec size()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "65e00902-5318-4d76-9b86-2479dcce7f52";
	out.name = "Size";

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec decay()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "aa47aa82-0e26-4d3b-8f60-ddb5d57353e7";
	out.name = "Decay";

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec mix()
{
	auto out = std_params::envelopes::mix();

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

Parameters::Parameters(blink::Plugin* plugin)
{
	env.size = plugin->add_parameter(parameters::envelopes::size());
	env.decay = plugin->add_parameter(parameters::envelopes::decay());
	env.mix = plugin->add_parameter(parameters::envelopes::mix());
}

} // spring_reverb