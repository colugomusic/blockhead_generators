#pragma once

#include <blink/standard_parameters/ms.hpp>

namespace compressor {
namespace params {
namespace attack {

static constexpr auto UUID { "568d2cba-2fbb-4536-83d1-44781f8b0cab" };

auto inline envelope()
{
	return blink::std_params::ms::envelope(0.001f, 10.0f, 0.1f);
}

auto inline envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Attack";

	out.envelope = envelope();

	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

auto inline envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.override_envelope = envelope();

	return out;
}

} // attack
} // params
} // compressor
