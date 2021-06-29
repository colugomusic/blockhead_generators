#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec SR()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Sample Rate";
	out.uuid = "1a606f6d-922f-4b5b-bd3a-a75fe291d447";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec BR()
{
	auto out = std_params::envelopes::generic::percentage();

	out.name = "Bit Reduction";
	out.uuid = "c64eeebc-65c8-44a9-a9d6-b139522df65b";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes
} // parameters
