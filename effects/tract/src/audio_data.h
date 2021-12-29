#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace tract {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Index)> index;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Diameter)> diameter;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_TongueIndex)> tongue_index;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_TongueDiameter)> tongue_diameter;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Quality)> quality;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // tract