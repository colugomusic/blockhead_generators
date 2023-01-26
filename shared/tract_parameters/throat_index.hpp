#pragma once

#include <blink/parameters/envelope_parameter.hpp>

namespace tract_params {
namespace throat_index {

static constexpr auto UUID { "64e246db-0d4d-4dc2-9d34-78a3a23070bc" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Throat Position";
	out.short_name = "Position";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	out.clamp_range = { -1.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_HostClamp;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::bipolar::envelope();

	return out;
}

} // throat_index
} // tract_params
