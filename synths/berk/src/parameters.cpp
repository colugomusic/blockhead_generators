#include "parameters.h"
#include <blink/standard_parameters/all.hpp>
#include "plugin.h"
#include "shared/tract_parameters.h"

using namespace blink;

namespace berk {
namespace parameters {

namespace envelopes {

auto amp()
{
	auto out = std_params::amp::envelope_parameter();

	out.envelope.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto pan()
{
	return std_params::pan::envelope_parameter();
}

auto pitch()
{
	auto out = std_params::pitch::envelope_parameter();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

auto buzz()
{
	auto out = std_params::formant::envelope_parameter();

	out.uuid = "a62d1f52-d493-4902-b92d-c09f5a92e8d2";
	out.name = "Buzz";

	return out;
}

} // envelopes
} // parameters

Parameters::Parameters(blink::Plugin* plugin)
{
	env.amp = plugin->add_parameter(parameters::envelopes::amp());
	env.pan = plugin->add_parameter(parameters::envelopes::pan());

	auto group_voice = plugin->add_group("Voice");
	{
		env.pitch = plugin->add_parameter(parameters::envelopes::pitch());
		env.buzz = plugin->add_parameter(parameters::envelopes::buzz());

		env.pitch->set_group_index(group_voice);
		env.buzz->set_group_index(group_voice);
	}

	auto group_throat = plugin->add_group("Throat");
	{
		env.index = plugin->add_parameter(tract_parameters::index());
		env.diameter = plugin->add_parameter(tract_parameters::diameter());

		env.index->set_group_index(group_throat);
		env.diameter->set_group_index(group_throat);
	}

	auto group_tongue = plugin->add_group("Tongue");
	{
		env.tongue_index = plugin->add_parameter(tract_parameters::tongue_index());
		env.tongue_diameter = plugin->add_parameter(tract_parameters::tongue_diameter());

		env.tongue_index->set_group_index(group_tongue);
		env.tongue_diameter->set_group_index(group_tongue);
	}

	auto group_fricatives = plugin->add_group("Fricatives");
	{
		env.fricative_intensity = plugin->add_parameter(tract_parameters::fricative_intensity());

		env.fricative_intensity->set_group_index(group_fricatives);
	}

	env.quality = plugin->add_parameter(tract_parameters::quality());
}

} // berk