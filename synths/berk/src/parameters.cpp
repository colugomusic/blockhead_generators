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

blink::EnvelopeSpec formant()
{
	return std_params::envelopes::formant();
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

	return out;
}

blink::EnvelopeSpec tongue_diameter()
{
	auto out = std_params::envelopes::generic::percentage();

	out.uuid = "a14bfe06-b84d-4778-b50c-7290478cb6b3";
	out.name = "Diameter";
	out.default_value = 1.0f;

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

} // envelopes
} // parameters