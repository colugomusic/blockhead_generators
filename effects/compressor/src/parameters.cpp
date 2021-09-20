#include "parameters.h"
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace parameters {
namespace envelopes {

EnvelopeSpec attack()
{
	auto out = std_params::envelopes::generic::ms(0.001f, 10.0f, 0.1f);

	out.name = "Attack";
	out.uuid = "568d2cba-2fbb-4536-83d1-44781f8b0cab";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec release()
{
	auto out = std_params::envelopes::generic::ms(0.001f, 100.0f, 10.0f);

	out.name = "Release";
	out.uuid = "990be907-d3f4-4b53-b65c-a5b4efb6ce97";

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec threshold()
{
	EnvelopeSpec out;

	out.name = "Threshold";
	out.uuid = "dca075fe-a713-4f5e-b7c6-61c01f1e700b";

	out.get_gridline = [](int index) -> float
	{
		return math::convert::linear_to_speed(float(index));
	};

	out.default_value = tweak::convert::db_to_linear(-20.0f);
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.to_string = std_params::amp::display;
	out.stepify = std_params::amp::stepify;

	out.value_slider = std_params::sliders::amp();

	out.value_slider.constrain = [](float v)
	{
		const auto db = tweak::convert::linear_to_db(v);

		if (db < -60.0f) return 0.0f;
		if (db > 0.0f) return 1.0f;

		return v;
	};

	out.range.min.default_value = tweak::convert::db_to_linear(-60.0f);
	out.range.min.to_string = std_params::amp::display;

	out.range.max.default_value = 1.0f;
	out.range.max.to_string = std_params::amp::display;

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec ratio()
{
	EnvelopeSpec out;

	out.name = "Ratio";
	out.uuid = "29bf39a4-b3c4-4a48-b577-dc252ce725c5";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	out.default_value = tweak::convert::ratio_to_linear(4.0f);
	out.get_gridline = [](int index) -> std::optional<float>
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
	out.search_binary = std_params::envelopes::generic_search_binary;
	out.search_forward = std_params::envelopes::generic_search_forward;
	out.stepify = [](float v)
	{
		return tweak::convert::ratio_to_linear(tweak::math::stepify<1000>(tweak::convert::linear_to_ratio(v)));
	};
	out.to_string = [](float v)
	{
		std::stringstream ss;

		ss << tweak::convert::linear_to_ratio(v) << " : 1";

		return ss.str();
	};

	out.range.min.default_value = 0.0f;
	out.range.min.to_string = out.to_string;

	out.range.max.default_value = 1.0f;
	out.range.max.to_string = out.to_string;

	out.value_slider.constrain = [](float v)
	{
		if (v < 0) return 0.0f;
		if (v > 1) return 1.0f;
		return v;
	};

	out.value_slider.decrement = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::decrement<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.value_slider.increment = [](float v, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::increment<1, 10>(tweak::convert::linear_to_ratio(v), precise));
	};

	out.value_slider.default_value = out.default_value;

	out.value_slider.drag = [](float value, int amount, bool precise)
	{
		return tweak::convert::ratio_to_linear(tweak::drag<float, 10, 100>(tweak::convert::linear_to_ratio(value), amount / 5, precise));
	};

	out.value_slider.from_string = [](const std::string& str) -> std::optional<float>
	{
		const auto number = tweak::find_positive_number<float>(str);

		if (!number) return number;

		return tweak::convert::ratio_to_linear(*number);
	};

	out.value_slider.stepify = out.stepify;
	out.value_slider.to_string = out.to_string;

	return out;
}

EnvelopeSpec knee()
{
	auto out = std_params::envelopes::generic::linear(0.0f, 10.0f, 1.0f);

	out.name = "Knee";
	out.uuid = "7680370d-2c77-4b87-8079-a9890d84cc2f";

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
	env.mix = plugin->add_parameter(std_params::envelopes::mix());

	options.stereo = plugin->add_parameter(parameters::options::stereo());
}

} // compressor
