#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace tract_params {
namespace tongue_index {

static constexpr auto UUID { "1059abc4-3a1c-42cc-a21b-235b2c08775b" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Tongue Position";
	out.short_name = "Position";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.envelope.default_value = 0.0f;

	out.clamp_range = { -1.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

inline auto envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::bipolar::envelope();

	return out;
}

} // tongue_index
} // tract_params
