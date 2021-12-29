#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace zap {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(zap::Parameters::Index::Env_Frequency)> frequency;
		blink::EnvelopeData<int(zap::Parameters::Index::Env_Resonance)> resonance;
		blink::EnvelopeData<int(zap::Parameters::Index::Env_Spread)> spread;
		blink::EnvelopeData<int(zap::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // zap