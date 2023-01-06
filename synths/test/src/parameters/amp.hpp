#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace test {
namespace params {
namespace amp {

static constexpr auto UUID { blink::std_params::amp::UUID };

auto envelope_parameter()
{
	auto out = blink::std_params::amp::envelope_parameter();

	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto inline slider_parameter() { return blink::std_params::amp::slider_parameter(); }

} // amp
} // params
} // test
