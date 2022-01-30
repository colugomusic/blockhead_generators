#pragma once

namespace compressor {
namespace params {
namespace knee {

static constexpr auto UUID { "7680370d-2c77-4b87-8079-a9890d84cc2f" };

auto inline envelope()
{
	return blink::std_params::linear::envelope(0.0f, 10.0f, 1.0f);
}

auto inline envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Knee";
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

} // knee
} // params
} // compressor
