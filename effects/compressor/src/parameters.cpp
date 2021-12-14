#include "parameters.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeParameterSpec attack()
{
	EnvelopeParameterSpec out;

	out.name = "Attack";
	out.uuid = "568d2cba-2fbb-4536-83d1-44781f8b0cab";

	out.envelope = std_params::ms::envelope(0.001f, 10.0f, 0.1f);
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec release()
{
	EnvelopeParameterSpec out;

	out.name = "Release";
	out.uuid = "990be907-d3f4-4b53-b65c-a5b4efb6ce97";

	out.envelope = std_params::ms::envelope(0.001f, 100.0f, 10.0f);
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec threshold()
{
	EnvelopeParameterSpec out;

	out.name = "Threshold";
	out.uuid = "dca075fe-a713-4f5e-b7c6-61c01f1e700b";

	out.envelope.get_gridline = [](int index) -> float
	{
		return math::convert::linear_to_speed(float(index));
	};

	out.envelope.default_value = tweak::convert::db_to_linear(-20.0f);
	out.envelope.search_binary = std_params::search::envelope_binary;
	out.envelope.search_forward = std_params::search::envelope_forward;
	out.envelope.to_string = std_params::amp::display;
	out.envelope.stepify = std_params::amp::stepify;

	out.envelope.value_slider = std_params::amp::slider();

	out.envelope.value_slider.constrain = [](float v)
	{
		const auto db = tweak::convert::linear_to_db(v);

		if (db < -60.0f) return 0.0f;
		if (db > 0.0f) return 1.0f;

		return v;
	};

	out.envelope.range.min.default_value = tweak::convert::db_to_linear(-60.0f);
	out.envelope.range.min.to_string = std_params::amp::display;

	out.envelope.range.max.default_value = 1.0f;
	out.envelope.range.max.to_string = std_params::amp::display;

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeParameterSpec ratio()
{
	EnvelopeParameterSpec out;

	out.name = "Ratio";
	out.uuid = "29bf39a4-b3c4-4a48-b577-dc252ce725c5";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	out.envelope.default_value = tweak::convert::ratio_to_linear(4.0f);
	out.envelope.get_gridline = [](int index) -> std::optional<float>
	{
		switch (index)
		{
			case 1: return tweak::convert::ratio_to_linear(1.0f);
			case 2: return tweak::convert::ratio_to_linear(2.0f);
			case 3: return tweak::convert::ratio_to_linear(4.0f);
			case 4: return tweak::convert::ratio_to_linear(8.0f);
			case 5: return tweak::convert::ratio_to_linear(16.0f);
			case 6: return tweak::convert::ratio_to_linear(32.0f);
			case 7: return tweak::convert::ratio_to_linear(64.0f);

			default: return std::optional<float>();
		}
	};
	out.envelope.search_binary = std_params::search::envelope_binary;
	out.envelope.search_forward = std_params::search::envelope_forward;
	out.envelope.stepify = [](float v)
	{
		return tweak::convert::ratio_to_linear(tweak::math::stepify<1000>(tweak::convert::linear_to_ratio(v)));
	};
	out.envelope.to_string = [](float v)
	{
		std::stringstream ss;

		ss << tweak::convert::linear_to_ratio(v) << " : 1";

		return ss.str();
	};

	out.envelope.range.min.default_value = 0.0f;
	out.envelope.range.min.to_string = out.envelope.to_string;

	out.envelope.range.max.default_value = 1.0f;
	out.envelope.range.max.to_string = out.envelope.to_string;

	out.envelope.value_slider.constrain = [](float v)
	{
		if (v < 0) return 0.0f;
		if (v > 1) return 1.0f;
		return v;
	};

	out.envelope.value_slider.decrement = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::decrement<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.envelope.value_slider.increment = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::increment<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.envelope.value_slider.default_value = out.envelope.default_value;

	out.envelope.value_slider.drag = [](float value, int amount, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::drag<float, 10, 100>(tweak::convert::linear_to_ratio(value), amount / 5, precise));
	};

	out.envelope.value_slider.from_string = [](const std::string& str) -> std::optional<float>
	{
		const auto number = tweak::find_positive_number<float>(str);

		if (!number) return number;

		return tweak::convert::ratio_to_linear(*number);
	};

	out.envelope.value_slider.stepify = out.envelope.stepify;
	out.envelope.value_slider.to_string = out.envelope.to_string;

	return out;
}

EnvelopeParameterSpec knee()
{
	EnvelopeParameterSpec out;

	out.name = "Knee";
	out.uuid = "7680370d-2c77-4b87-8079-a9890d84cc2f";
	out.envelope = std_params::linear::envelope(0.0f, 10.0f, 1.0f);
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes

namespace options {

OptionSpec stereo()
{
	OptionSpec out;

	out.name = "Processing Mode";
	out.uuid = "47601288-d3d0-4d35-9cd3-f2e985b49cde";

	out.default_index = 0;
	out.options.push_back("Mono");
	out.options.push_back("Stereo");

	return out;
}

} // options
} // parameters

namespace compressor {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.attack = plugin->add_parameter(parameters::envelopes::attack());
	env.release = plugin->add_parameter(parameters::envelopes::release());
	env.threshold = plugin->add_parameter(parameters::envelopes::threshold());
	env.ratio = plugin->add_parameter(parameters::envelopes::ratio());
	env.knee = plugin->add_parameter(parameters::envelopes::knee());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
	options.stereo = plugin->add_parameter(parameters::options::stereo());
}

} // compressor
