#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include "plugin.h"

namespace berk {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Pan)> pan;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Buzz)> buzz;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Index)> index;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Diameter)> diameter;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_TongueIndex)> tongue_index;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_TongueDiameter)> tongue_diameter;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_FricativeIntensity)> fricative_intensity;
		blink::EnvelopeData<int(berk::Parameters::Index::Env_Quality)> quality;

		Envelopes(const Parameters::Envelopes& parameters, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(berk::Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(berk::Parameters::Index::Sld_Pan)> pan;
		blink::SliderData<int(berk::Parameters::Index::Sld_Pitch)> pitch;

		Sliders(const Parameters::Sliders& paramters, const blink_ParameterData* parameter_data);
	} sliders;

	AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data);
};

} // berk