#include "parameters.h"
#include <blink/standard_parameters.hpp>
#include "plugin.h"

using namespace blink;

namespace parameters {
namespace envelopes {

blink::EnvelopeSpec amp()
{
	auto out = std_params::envelopes::amp();

	out.default_value = 0.5f;
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec pan()
{
	return std_params::envelopes::pan();
}

blink::EnvelopeSpec pitch()
{
	auto out = std_params::envelopes::pitch();

	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec buzz()
{
	auto out = std_params::envelopes::formant();

	out.uuid = "a62d1f52-d493-4902-b92d-c09f5a92e8d2";
	out.name = "Buzz";

	return out;
}

blink::EnvelopeSpec index()
{
	auto out = std_params::envelopes::generic::percentage_bipolar();

	out.uuid = "64e246db-0d4d-4dc2-9d34-78a3a23070bc";
	out.name = "Index";
	out.flags |= blink_EnvelopeFlags_DefaultActive;

	return out;
}

blink::EnvelopeSpec diameter()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "ac0a8acd-29a4-42af-b75b-1d7ee2684b25";
	out.name = "Diameter";
	out.flags |= blink_EnvelopeFlags_DefaultActive;
	out.default_value = 1.0f;

	out.get_gridline = [](int index) -> std::optional<float>
	{
		if (index == -1) return 0.25f;

		return std::optional<float>();
	};

	return out;
}

blink::EnvelopeSpec tongue_index()
{
	auto out = std_params::envelopes::generic::percentage_bipolar();

	out.uuid = "1059abc4-3a1c-42cc-a21b-235b2c08775b";
	out.name = "Index";
	out.default_value = 0.0f;

	return out;
}

blink::EnvelopeSpec tongue_diameter()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "a14bfe06-b84d-4778-b50c-7290478cb6b3";
	out.name = "Diameter";
	out.default_value = 0.5f;

	return out;
}

blink::EnvelopeSpec fricative_intensity()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "53c94f7f-d6b7-497e-8deb-9acd35f0d60e";
	out.name = "Intensity";
	out.default_value = 1.0f;

	return out;
}

blink::EnvelopeSpec quality()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "0111ffd7-78d1-4604-8d7b-956c34e280c3";
	out.name = "Quality";
	out.default_value = 0.5f;

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
		env.index = plugin->add_parameter(parameters::envelopes::index());
		env.diameter = plugin->add_parameter(parameters::envelopes::diameter());

		env.pitch->set_group_index(group_voice);
		env.buzz->set_group_index(group_voice);
		env.index->set_group_index(group_voice);
		env.diameter->set_group_index(group_voice);
	}

	auto group_tongue = plugin->add_group("Tongue");
	{
		env.tongue_index = plugin->add_parameter(parameters::envelopes::tongue_index());
		env.tongue_diameter = plugin->add_parameter(parameters::envelopes::tongue_diameter());

		env.tongue_index->set_group_index(group_tongue);
		env.tongue_diameter->set_group_index(group_tongue);
	}

	auto group_fricatives = plugin->add_group("Fricatives");
	{
		env.fricative_intensity = plugin->add_parameter(parameters::envelopes::fricative_intensity());

		env.fricative_intensity->set_group_index(group_fricatives);
	}

	env.quality = plugin->add_parameter(parameters::envelopes::quality());
}