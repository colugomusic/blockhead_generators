#pragma once

#include <blink/envelope_data.hpp>
#include <blink/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace template {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Example)> example;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // template