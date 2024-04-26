#pragma once

#include <blink/standard_parameters/ms.hpp>

namespace compressor {
namespace params {
namespace release {

static constexpr auto UUID { "990be907-d3f4-4b53-b65c-a5b4efb6ce97" };

auto inline envelope()
{
	return blink::std_params::ms::envelope(0.001f, 100.0f, 10.0f);
}

auto inline envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Release";

	out.envelope = envelope();

	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_CanManipulate;

	out.override_envelope = envelope();

	return out;
}

} // release
} // params
} // compressor