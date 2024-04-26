#pragma once

#include <blink/standard_parameters/percentage.hpp>
#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>

namespace resonator {
namespace params {
namespace fm_amount {

static constexpr auto UUID { "d1b4d493-35f9-408b-982c-4474517a968c" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.name = "FM Amount";
	out.short_name = "Amount";
	out.uuid = UUID;
	out.envelope = blink::std_params::percentage::envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { 0.0f, 2.0f };

	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_HostClamp;

	return out;
}

} // fm_amount
} // params
} // resonator
