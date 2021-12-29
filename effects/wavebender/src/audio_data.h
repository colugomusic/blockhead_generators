#pragma once

#include <blink/envelope_data.hpp>
#include <blink/parameters/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace wavebender {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_Tilt)> tilt;
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_XFadeSize)> xfade_size;
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_Bubble)> bubble;
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_Smoother)> smoother;
		blink::EnvelopeData<int(wavebender::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Options
	{
		blink::OptionData<int(wavebender::Parameters::Index::Opt_XFadeMode)> xfade_mode;

		Options(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} options;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // wavebender