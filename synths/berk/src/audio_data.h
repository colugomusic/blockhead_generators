#pragma once

#include <blink/envelope_data.hpp>
#include <blink/synth.hpp>
#include "plugin.h"

namespace berk {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Amp)> amp;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Pitch)> pitch;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Buzz)> buzz;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Index)> index;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Diameter)> diameter;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_TongueIndex)> tongue_index;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_TongueDiameter)> tongue_diameter;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_FricativeIntensity)> fricative_intensity;
		blink::EnvelopeData<int(Berk::ParameterIndex::Env_Quality)> quality;

		Envelopes(const Berk* plugin, const blink_SynthBuffer* buffer);
	} envelopes;

	AudioData(const Berk* plugin, const blink_SynthBuffer* buffer);
};

} // berk