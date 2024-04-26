#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace spring_reverb {
namespace params {
namespace size {

static constexpr auto UUID { "65e00902-5318-4d76-9b86-2479dcce7f52" };

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
	out.name = "Size";

	out.envelope = envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_HostClamp;

	return out;
}

} // size
} // params
} // spring_reverb
