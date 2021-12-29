#pragma once

#include <blink/envelope_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace saturator {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(saturator::Parameters::Index::Env_Drive)> drive;
		blink::EnvelopeData<int(saturator::Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(saturator::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // saturator