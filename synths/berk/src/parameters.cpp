#include "parameters.h"
#include "plugin.h"
#include "parameters/amp.hpp"
#include "parameters/buzz.hpp"
#include "parameters/pan.hpp"
#include "parameters/pitch.hpp"
#include "shared/tract_parameters/fricative_intensity.hpp"
#include "shared/tract_parameters/quality.hpp"
#include "shared/tract_parameters/throat_diameter.hpp"
#include "shared/tract_parameters/throat_index.hpp"
#include "shared/tract_parameters/tongue_diameter.hpp"
#include "shared/tract_parameters/tongue_index.hpp"

using namespace blink;

namespace berk {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.amp = plugin->add_parameter(params::amp::envelope_parameter());
	env.pan = plugin->add_parameter(params::pan::envelope_parameter());

	auto group_voice = "Voice";
	{
		env.pitch = plugin->add_parameter(params::pitch::envelope_parameter());
		env.buzz = plugin->add_parameter(params::buzz::envelope_parameter());

		env.pitch->set_group_name(group_voice);
		env.buzz->set_group_name(group_voice);
	}

	auto group_throat = "Throat";
	{
		env.index = plugin->add_parameter(tract_params::throat_index::envelope_parameter());
		env.diameter = plugin->add_parameter(tract_params::throat_diameter::envelope_parameter());

		env.index->set_group_name(group_throat);
		env.diameter->set_group_name(group_throat);
	}

	auto group_tongue = "Tongue";
	{
		env.tongue_index = plugin->add_parameter(tract_params::tongue_index::envelope_parameter());
		env.tongue_diameter = plugin->add_parameter(tract_params::tongue_diameter::envelope_parameter());

		env.tongue_index->set_group_name(group_tongue);
		env.tongue_diameter->set_group_name(group_tongue);
	}

	auto group_fricatives = "Fricatives";
	{
		env.fricative_intensity = plugin->add_parameter(tract_params::fricative_intensity::envelope_parameter());

		env.fricative_intensity->set_group_name(group_fricatives);
	}

	env.quality = plugin->add_parameter(tract_params::quality::envelope_parameter());

	sliders.amp = plugin->add_parameter(params::amp::slider_parameter());
	sliders.pan = plugin->add_parameter(params::pan::slider_parameter());
	sliders.pitch = plugin->add_parameter(params::pitch::slider_parameter());
}

} // berk