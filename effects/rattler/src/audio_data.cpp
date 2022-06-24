#include "audio_data.h"

namespace rattler {

AudioData::Envelopes::Envelopes(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: amp { parameters.env.amp->envelope, parameter_data }
	, time { parameters.env.time->envelope, parameter_data }
	, feedback { parameters.env.feedback->envelope, parameter_data }
	, width { parameters.env.width->envelope, parameter_data }
{
}

AudioData::Sliders::Sliders(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: amp { parameters.sliders.amp->slider, parameter_data }
	, width { parameters.sliders.width->slider, parameter_data }
{
}

AudioData::AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: envelopes { parameters, parameter_data }
	, sliders { parameters, parameter_data }
{
}

} // rattler
