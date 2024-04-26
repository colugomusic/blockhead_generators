#pragma once

#include <blink/standard_parameters/amp.hpp>

namespace compressor {
namespace params {
namespace threshold {

static constexpr auto UUID { "dca075fe-a713-4f5e-b7c6-61c01f1e700b" };

auto inline envelope()
{
	blink::EnvelopeSpec out;

	out.get_gridline = [](int index) -> float
	{
		return blink::math::convert::linear_to_speed(float(index));
	};

	out.default_value = tweak::convert::db_to_linear(-20.0f);
	out.searcher.binary = blink::std_params::search::float_points_binary;
	out.searcher.forward = blink::std_params::search::float_points_forward;
	out.to_string = blink::std_params::amp::display;
	out.stepify = blink::std_params::amp::stepify;

	out.value_slider = blink::std_params::amp::slider();

	out.value_slider.constrain = [](float v)
	{
		const auto db = tweak::convert::linear_to_db(v);

		if (db < -60.0f) return 0.0f;
		if (db > 0.0f) return 1.0f;

		return v;
	};

	out.range.min.default_value = tweak::convert::db_to_linear(-60.0f);
	out.range.min.to_string = blink::std_params::amp::display;

	out.range.max.default_value = 1.0f;
	out.range.max.to_string = blink::std_params::amp::display;

	return out;
}

auto inline envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Threshold";

	out.envelope = envelope();
	out.override_envelope = envelope();

	out.flags |= blink_ParamFlags_DefaultActive;

	return out;
}

} // threshold
} // params
} // compressor
