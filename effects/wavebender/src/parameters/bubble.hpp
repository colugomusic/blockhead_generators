#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace wavebender {
namespace params {
namespace bubble {

static constexpr auto UUID { "2306f951-daca-4833-a175-258dc5407b02" };

auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Bubble";
	out.envelope = blink::std_params::percentage::envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_HostClamp;

	return out;
}

} // bubble
} // params
} // wavebender
