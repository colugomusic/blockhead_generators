#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace resonator {
namespace params {
namespace harmonics_width {

static constexpr auto UUID { "2f95dc68-9ae3-4e12-8ef1-81c36201b995" };

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
	out.name = "Harmonics Width";
	out.short_name = "Width";
	out.envelope = envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // harmonics_width
} // params
} // resonator
