#pragma once

#include <blink/parameters/envelope_manipulator_target_spec.hpp>
#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace wavebender {
namespace params {
namespace xfade_size {

static constexpr auto UUID { "d9f04877-408d-4411-8983-21f66fc149a8" };

auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Crossfade";

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();
	out.envelope = blink::std_params::percentage::envelope();
	out.envelope.default_value = 0.25f;
	out.options.push_back(blink_Index(Parameters::Index::Opt_XFadeMode));
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // xfade_size
} // params
} // wavebender
