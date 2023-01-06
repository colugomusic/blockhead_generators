#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace resonator {
namespace params {
namespace harmonics_scale_snap_amount {

static constexpr auto UUID { "de8c7343-360a-41fd-9788-ae2398d0a872" };

inline auto envelope()
{
	auto out { blink::std_params::percentage::envelope() };

	out.default_value = 1.0f;

	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Harmonics Scale Snap Amount";
	out.short_name = "Snap Amount";
	out.long_desc = "Controls the amount of scale snapping for the harmonics";
	out.envelope = envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // harmonics_scale_snap_amount
} // params
} // resonator
