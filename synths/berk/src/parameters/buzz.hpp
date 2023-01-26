#pragma once

#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace berk {
namespace params {
namespace buzz {

static constexpr auto UUID { "a62d1f52-d493-4902-b92d-c09f5a92e8d2" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Buzz";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.clamp_range = { -1.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::bipolar::envelope();

	return out;
}

} // buzz
} // params
} // berk
