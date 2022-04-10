#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace gainer {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Pan)> pan;

		Envelopes(const Parameters& parameters, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Pan)> pan;

		Sliders(const Parameters& parameters, const blink_ParameterData* parameter_data);
	} sliders;

	AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data);
};

} // gainer