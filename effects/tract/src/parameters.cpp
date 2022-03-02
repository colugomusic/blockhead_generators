#include "parameters.h"
#include <blink/standard_parameters/all.hpp>
#include "plugin.h"
#include "shared/tract_parameters/fricative_intensity.hpp"
#include "shared/tract_parameters/quality.hpp"
#include "shared/tract_parameters/throat_diameter.hpp"
#include "shared/tract_parameters/throat_index.hpp"
#include "shared/tract_parameters/tongue_diameter.hpp"
#include "shared/tract_parameters/tongue_index.hpp"

using namespace blink;

namespace tract {

Parameters::Parameters(blink::Plugin* plugin)
{
	auto group_voice = plugin->add_group("Voice");
	{
		env.index = plugin->add_parameter(tract_params::throat_index::envelope_parameter());
		env.diameter = plugin->add_parameter(tract_params::throat_diameter::envelope_parameter());

		env.index->set_group_index(group_voice);
		env.diameter->set_group_index(group_voice);
	}

	auto group_tongue = plugin->add_group("Tongue");
	{
		env.tongue_index = plugin->add_parameter(tract_params::tongue_index::envelope_parameter());
		env.tongue_diameter = plugin->add_parameter(tract_params::tongue_diameter::envelope_parameter());

		env.tongue_index->set_group_index(group_tongue);
		env.tongue_diameter->set_group_index(group_tongue);
	}

	env.quality = plugin->add_parameter(tract_params::quality::envelope_parameter());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());

	plugin->add_manipulator_target(tract_params::throat_index::UUID, tract_params::throat_index::envelope_manipulator_target());
	plugin->add_manipulator_target(tract_params::throat_diameter::UUID, tract_params::throat_diameter::envelope_manipulator_target());
	plugin->add_manipulator_target(tract_params::tongue_index::UUID, tract_params::tongue_index::envelope_manipulator_target());
	plugin->add_manipulator_target(tract_params::tongue_diameter::UUID, tract_params::tongue_diameter::envelope_manipulator_target());
	plugin->add_manipulator_target(tract_params::quality::UUID, tract_params::quality::envelope_manipulator_target());
	plugin->add_manipulator_target(std_params::mix::UUID, std_params::mix::envelope_manipulator_target());
}

} // tract