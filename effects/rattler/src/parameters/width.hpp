#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace rattler {
namespace params {
namespace width {

static constexpr auto UUID { "aec5e808-dcbb-424c-95a6-ba4749edccc4" };

auto inline envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Width";
	out.envelope = blink::std_params::percentage::bipolar::envelope();
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;

	return out;
}

auto inline slider_parameter()
{
	blink::SliderParameterSpec<float> out;

	out.uuid = UUID;
	out.name = "Width";
	out.slider = blink::std_params::percentage::bipolar::slider();

	out.flags |= blink_SliderFlags_CanManipulate;

	return out;
}

auto inline envelope_manipulator_target()
{
	blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = blink::std_params::percentage::bipolar::envelope();

	return out;
}

} // feedback
} // params
} // rattler
