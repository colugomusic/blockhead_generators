#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace ring_modulator {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // ring_modulator