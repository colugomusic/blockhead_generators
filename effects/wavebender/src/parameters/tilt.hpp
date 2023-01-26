#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace wavebender {
namespace params {
namespace tilt {

static constexpr auto UUID { "196b2999-7979-4b2e-bbe2-2b8684d4af80" };

auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Tilt";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { -1.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_HostClamp;

	return out;
}

} // tilt
} // params
} // wavebender
