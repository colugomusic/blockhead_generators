#include "audio_data.h"

namespace echo {

AudioData::Envelopes::Envelopes(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: amp { parameters.env.amp->envelope, parameter_data }
	, pan { parameters.env.pan->envelope, parameter_data }
{
}

AudioData::Sliders::Sliders(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: amp { parameters.sliders.amp->slider, parameter_data }
	, pan { parameters.sliders.pan->slider, parameter_data }
{
}

AudioData::AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: envelopes { parameters, parameter_data }
	, sliders { parameters, parameter_data }
{
}

} // echo
