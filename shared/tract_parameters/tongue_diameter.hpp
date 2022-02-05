#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace tract_params {
namespace tongue_diameter {

static constexpr auto UUID { "a14bfe06-b84d-4778-b50c-7290478cb6b3" };

inline auto envelope()
{
	auto out { blink::std_params::percentage::envelope() };

	out.default_value = 0.5f;

	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Tongue Diameter";
	out.short_name = "Diameter";
	out.envelope = envelope();

	out.clamp_range = { 0.0f, 1.0f };

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
	out.override_envelope = envelope();

	return out;
}

} // tongue_diameter
} // tract_params
