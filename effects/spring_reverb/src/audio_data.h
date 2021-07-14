#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace spring_reverb {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(spring_reverb::Parameters::Index::Env_Size)> size;
		blink::EnvelopeData<int(spring_reverb::Parameters::Index::Env_Decay)> decay;
		blink::EnvelopeData<int(spring_reverb::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // spring_reverb