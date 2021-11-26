#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec time()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Time";
	out.uuid = "20087ff3-c3b0-45ba-bd0c-e77f1a5f70b2";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec feedback()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Feedback";
	out.uuid = "18bc8797-0414-4830-8a54-a553750f9482";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec dampener()
{
	auto out = std_params::envelopes::filter_frequency();

	out.name = "Dampener";
	out.uuid = "b9fe70f2-ee36-4b67-9232-543c8ac40863";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters

namespace delay {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.time = plugin->add_parameter(parameters::envelopes::time());
	env.feedback = plugin->add_parameter(parameters::envelopes::feedback());
	env.dampener = plugin->add_parameter(parameters::envelopes::dampener());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // delay
