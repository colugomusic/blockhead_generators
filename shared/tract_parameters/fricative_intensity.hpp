#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace tract_params {
namespace fricative_intensity {

static constexpr auto UUID { "53c94f7f-d6b7-497e-8deb-9acd35f0d60e" };

inline auto envelope()
{
	auto out { blink::std_params::percentage::envelope() };

	out.default_value = 1.0f;
	
	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Fricative Intensity";
	out.short_name = "Intensity";
	out.envelope = envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();

	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

} // fricative_intensity
} // tract_params
