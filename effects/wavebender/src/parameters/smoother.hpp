#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace wavebender {
namespace params {
namespace smoother {

static constexpr auto UUID { "357b372b-f5d1-4e41-be6c-24dda95d6ad4" };

auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Smoother";
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();
	out.envelope = blink::std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // smoother
} // params
} // wavebender
