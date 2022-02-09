#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace lofi {
namespace params {
namespace sample_rate {

static constexpr auto UUID { "1a606f6d-922f-4b5b-bd3a-a75fe291d447" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.name = "Sample Rate";
	out.uuid = UUID;

	out.envelope = blink::std_params::percentage::envelope();
	out.envelope.default_value = 1.0f;

	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;
	out.flags |= blink_EnvelopeFlags_HostClamp;

	return out;
}

inline auto envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	return out;
};

} // sample_rate
} // params
} // lofi
