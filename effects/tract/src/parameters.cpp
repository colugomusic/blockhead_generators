#include "parameters.h"
#include <blink/standard_parameters/all.hpp>
#include "plugin.h"
#include "shared/tract_parameters/quality.hpp"
#include "shared/tract_parameters/throat_diameter.hpp"
#include "shared/tract_parameters/throat_index.hpp"
#include "shared/tract_parameters/tongue_diameter.hpp"
#include "shared/tract_parameters/tongue_index.hpp"

using namespace blink;

namespace tract {

Parameters::Parameters(blink::Plugin* plugin)
{
	auto group_voice = "Voice";
	{
		env.index = plugin->add_parameter(tract_params::throat_index::envelope_parameter());
		env.diameter = plugin->add_parameter(tract_params::throat_diameter::envelope_parameter());

		env.index->set_group_name(group_voice);
		env.diameter->set_group_name(group_voice);
	}

	auto group_tongue = "Tongue";
	{
		env.tongue_index = plugin->add_parameter(tract_params::tongue_index::envelope_parameter());
		env.tongue_diameter = plugin->add_parameter(tract_params::tongue_diameter::envelope_parameter());

		env.tongue_index->set_group_name(group_tongue);
		env.tongue_diameter->set_group_name(group_tongue);
	}

	env.quality = plugin->add_parameter(tract_params::quality::envelope_parameter());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // tract