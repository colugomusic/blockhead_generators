#include "parameters.h"
#include <blink/standard_parameters.hpp>
#include "plugin.h"
#include "shared/tract_parameters.h"

using namespace blink;

namespace tract {

Parameters::Parameters(blink::Plugin* plugin)
{
	auto group_voice = plugin->add_group("Voice");
	{
		env.index = plugin->add_parameter(tract_parameters::index());
		env.diameter = plugin->add_parameter(tract_parameters::diameter());

		env.index->set_group_index(group_voice);
		env.diameter->set_group_index(group_voice);
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
		auto spec_fricative_intensity = tract_parameters::fricative_intensity();

		spec_fricative_intensity.default_value = 0.0f;

		env.fricative_intensity = plugin->add_parameter(spec_fricative_intensity);

		env.fricative_intensity->set_group_index(group_fricatives);
	}

	env.quality = plugin->add_parameter(tract_parameters::quality());
	env.mix = plugin->add_parameter(std_params::envelopes::mix());
}

} // tract