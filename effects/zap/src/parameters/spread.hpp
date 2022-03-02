#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace zap {
namespace params {
namespace spread {

static constexpr auto UUID { "7112fe4a-7d61-4b48-acd3-0c8f8fa34fa5" };

auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Spread";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.clamp_range = { -1.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

auto inline envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::bipolar::envelope();

	return out;
}

} // spread
} // params
} // zap
