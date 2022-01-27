#pragma once

#include "../convert.h"
#include <blink/standard_parameters/percentage.hpp>

namespace fudge {
namespace params {
namespace grain_size {

static constexpr auto UUID { "2a9c7ec6-68b4-40c1-aad9-b3c010cd9717" };

auto stepify(float v) -> float
{
	return v;
}

auto snap_value(float v, float step_size, float snap_amount)
{
	return stepify(tweak::snap_value(v, step_size, snap_amount));
}

float constrain(float v)
{
	return std::clamp(v, 0.0f, 1.0f);
};

auto increment(float v, bool precise)
{
	return constrain(stepify(tweak::increment<100, 1000>(v, precise)));
};

auto decrement(float v, bool precise)
{
	return constrain(stepify(tweak::decrement<100, 1000>(v, precise)));
};

auto drag(float v, int amount, bool precise) -> float
{
	return constrain(stepify(tweak::drag<float, 100, 1000>(v, amount / 5, precise)));
};

auto to_string(float v)
{
	std::stringstream ss;

	ss << convert::linear_to_ms(v) << " ms";

	return ss.str();
}

auto from_string(const std::string& str) -> std::optional<float>
{
	auto value = tweak::find_number<float>(str);

	if (!value) return std::optional<float>();

	return convert::ms_to_linear(*value);
}

auto inline slider()
{
	::blink::SliderSpec<float> out;

	out.constrain = constrain;
	out.increment = increment;
	out.decrement = decrement;
	out.drag = drag;
	out.to_string = to_string;
	out.from_string = from_string;
	out.default_value = 0.5f;

	return out;
}

auto inline envelope()
{
	::blink::EnvelopeSpec out;

	out.show_grid_labels = false;
	out.default_value = 0.5f;
	out.searcher.binary = ::blink::std_params::search::float_points_binary;
	out.searcher.forward = ::blink::std_params::search::float_points_forward;
	out.stepify = stepify;

	out.value_slider = slider();

	out.range.min.default_value = 0.0f;
	out.range.min.to_string = to_string;
	out.range.max.default_value = 1.0f;
	out.range.max.to_string = to_string;
	out.to_string = to_string;

	return out;
}

auto inline envelope_parameter()
{
	::blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Grain Size";
	out.short_name = "Size";

	out.envelope = envelope();

	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_EnvelopeFlags_HostClamp;
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.flags |= blink_EnvelopeFlags_CanManipulate;
	out.flags |= blink_EnvelopeFlags_IsManipulatorTarget;

	return out;
}

auto inline envelope_manipulator_target()
{
	::blink::EnvelopeManipulatorTargetSpec out;

	out.offset_envelope = ::blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();

	return out;
}

} // grain_size
} // params
} // fudge
