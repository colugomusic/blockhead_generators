#pragma once

#include <blink/parameters/envelope_parameter_spec.hpp>
#include <blink/standard_parameters/percentage.hpp>

namespace test {
namespace params {
namespace carrier {
namespace waveform {

static constexpr auto UUID { "337b8f6f-1d7a-41a7-ab05-802b23887b7a" };

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Waveform";
	out.envelope = blink::std_params::percentage::envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::envelope();

	return out;
}

} // waveform
} // carrier
} // params
} // test
