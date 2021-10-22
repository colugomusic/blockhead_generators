#pragma once

#include <blink/envelope_data.hpp>
#include <blink/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace resonator {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Dispersion)> dispersion;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // resonator