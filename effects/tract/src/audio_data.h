#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect.hpp>
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
		blink::EnvelopeData<int(tract::Parameters::Index::Env_FricativeIntensity)> fricative_intensity;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Quality)> quality;
		blink::EnvelopeData<int(tract::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Tract* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	AudioData(const Tract* plugin, const blink_EffectBuffer* buffer);
};

} // tract