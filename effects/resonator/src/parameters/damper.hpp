#pragma once

#include <blink/standard_parameters/percentage.hpp>
#include <blink/parameters/envelope_manipulator_target.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>

namespace resonator {
namespace params {
namespace damper {

static constexpr auto UUID { "94b001e1-0796-489a-a244-e3d6fcbcfb9c" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.name = "Damper";
	out.uuid = UUID;
	out.envelope = blink::std_params::percentage::envelope();
	out.envelope.default_value = 0.5f;
	out.clamp_range = { 0.0f, 1.0f };
	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_HostClamp;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	return out;
}

} // damper
} // params
} // resonator
