#pragma once

#include <blink/envelope_data.hpp>
#include "plugin.h"

namespace berk {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Buzz)> buzz;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Index)> index;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Diameter)> diameter;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_TongueIndex)> tongue_index;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_TongueDiameter)> tongue_diameter;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_FricativeIntensity)> fricative_intensity;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Quality)> quality;

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // berk