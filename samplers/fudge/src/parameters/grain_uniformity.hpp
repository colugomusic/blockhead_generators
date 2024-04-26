#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace fudge {
namespace params {
namespace grain_uniformity {

static constexpr auto UUID { "83c352fb-35b5-4cb0-a6f7-05d082b56a16" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Grain Uniformity";
	out.short_name = "Uniformity";
	out.envelope = blink::std_params::percentage::envelope();
	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_HostClamp;

	return out;
}

} // grain_uniformity
} // params
} // fudge
