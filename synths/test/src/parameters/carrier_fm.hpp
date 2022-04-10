#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace test {
namespace params {
namespace carrier {
namespace fm {

static constexpr auto UUID { "05ffbc42-d2aa-4746-905e-44d4373b4345" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Carrier FM Amount";
	out.short_name = "FM Amount";
	out.envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { 0.0f, 2.0f };
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

inline auto envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	return out;
}

} // fm
} // carrier
} // params
} // test
