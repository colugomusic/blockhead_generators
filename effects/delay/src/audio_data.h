#pragma once

#include <blink/envelope_data.hpp>
#include <blink/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace delay {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Time)> time;
		blink::EnvelopeData<int(Parameters::Index::Env_Feedback)> feedback;
		blink::EnvelopeData<int(Parameters::Index::Env_Dampener)> dampener;
		blink::EnvelopeData<int(Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // delay