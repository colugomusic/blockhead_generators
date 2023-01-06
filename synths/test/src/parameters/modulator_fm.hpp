#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace test {
namespace params {
namespace modulator {
namespace fm {

static constexpr auto UUID { "343689ff-d1bf-48f0-96bf-7a6689568ef5" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Modulator FM Amount";
	out.short_name = "FM Amount";
	out.envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { 0.0f, 2.0f };
	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	return out;
}

} // fm
} // modulator
} // params
} // test
