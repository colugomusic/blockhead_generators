#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace lofi {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(lofi::Parameters::Index::Env_BR)> BR;
		blink::EnvelopeData<int(lofi::Parameters::Index::Env_SR)> SR;
		blink::EnvelopeData<int(lofi::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // lofi