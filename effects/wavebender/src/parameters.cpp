#include "parameters.h"
#include <blink/standard_parameters.hpp>
#include "plugin.h"

using namespace blink;

namespace wavebender {

namespace parameters {
namespace envelopes {

EnvelopeSpec bubble()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.name = "Bubble";
	out.uuid = "2306f951-daca-4833-a175-258dc5407b02";

	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec tilt()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage_bipolar();

	out.name = "Tilt";
	out.uuid = "196b2999-7979-4b2e-bbe2-2b8684d4af80";

	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec pitch()
{
	EnvelopeSpec out = std_params::envelopes::pitch();

	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec xfade_size()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.name = "Crossfade";
	out.uuid = "d9f04877-408d-4411-8983-21f66fc149a8";

	out.default_value = 0.25f;
	out.options.push_back(blink_Index(Parameters::Index::Opt_XFadeMode));

	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

EnvelopeSpec smoother()
{
	EnvelopeSpec out = std_params::envelopes::generic::percentage();

	out.name = "Smoother";
	out.uuid = "357b372b-f5d1-4e41-be6c-24dda95d6ad4";

	out.default_value = 0.5f;
	out.flags = blink_EnvelopeFlags_DefaultActive;

	return out;
}

} // envelopes

namespace options {

OptionSpec xfade_mode()
{
	OptionSpec out;

	out.name = "Crossfade Mode";
	out.uuid = "32e0b599-2dc9-4dfc-b7fa-e284e72289b5";

	out.default_index = 1;
	out.options.push_back("Static");
	out.options.push_back("Dynamic");

	return out;
}

} // options
} // parameters

Parameters::Parameters(blink::Plugin* plugin)
{
	env.tilt = plugin->add_parameter(parameters::envelopes::tilt());
	env.xfade_size = plugin->add_parameter(parameters::envelopes::xfade_size());
	env.pitch = plugin->add_parameter(parameters::envelopes::pitch());
	env.bubble = plugin->add_parameter(parameters::envelopes::bubble());
	env.smoother = plugin->add_parameter(parameters::envelopes::smoother());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());

	opt.xfade_mode = plugin->add_parameter(parameters::options::xfade_mode());
}

} // wavebender