#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace lowpass {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Freq)> freq;
		blink::EnvelopeData<int(Parameters::Index::Env_Res)> res;
		blink::EnvelopeData<int(Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // lowpass