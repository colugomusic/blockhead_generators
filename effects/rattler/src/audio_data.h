#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace rattler {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Time)> time;
		blink::EnvelopeData<int(Parameters::Index::Env_Feedback)> feedback;
		blink::EnvelopeData<int(Parameters::Index::Env_Width)> width;
		blink::EnvelopeData<int(Parameters::Index::Env_Dry)> dry;
		blink::EnvelopeData<int(Parameters::Index::Env_Wet)> wet;

		Envelopes(const Parameters& parameters, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(Parameters::Index::Sld_Dry)> dry;
		blink::SliderData<int(Parameters::Index::Sld_Wet)> wet;
		blink::SliderData<int(Parameters::Index::Sld_Width)> width;

		Sliders(const Parameters& parameters, const blink_ParameterData* parameter_data);
	} sliders;

	AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data);
};

} // rattler