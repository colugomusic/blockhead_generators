#include "audio_data.h"

namespace rattler {

AudioData::Envelopes::Envelopes(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: time { parameters.env.time->envelope, parameter_data }
	, feedback { parameters.env.feedback->envelope, parameter_data }
	, width { parameters.env.width->envelope, parameter_data }
	, dry { parameters.env.dry->envelope, parameter_data }
	, wet { parameters.env.wet->envelope, parameter_data }
{
}

AudioData::Sliders::Sliders(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: dry { parameters.sliders.dry->slider, parameter_data }
	, wet { parameters.sliders.wet->slider, parameter_data }
	, width { parameters.sliders.width->slider, parameter_data }
{
}

AudioData::AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: envelopes { parameters, parameter_data }
	, sliders { parameters, parameter_data }
{
}

} // rattler
