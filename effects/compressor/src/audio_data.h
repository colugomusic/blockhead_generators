#pragma once

#include <blink/envelope_data.hpp>
#include <blink/parameters/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace compressor {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Attack)> attack;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Release)> release;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Threshold)> threshold;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Ratio)> ratio;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Knee)> knee;
		blink::EnvelopeData<int(compressor::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Options
	{
		blink::OptionData<int(compressor::Parameters::Index::Opt_Stereo)> stereo;

		Options(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} options;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // compressor