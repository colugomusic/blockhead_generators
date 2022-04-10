#include "audio_data.h"

namespace berk {

AudioData::Envelopes::Envelopes(const Parameters::Envelopes& parameters, const blink_ParameterData* parameter_data)
	: amp(parameters.amp->envelope, parameter_data)
	, pan(parameters.pan->envelope, parameter_data)
	, pitch(parameters.pitch->envelope, parameter_data)
	, buzz(parameters.buzz->envelope, parameter_data)
	, index(parameters.index->envelope, parameter_data)
	, diameter(parameters.diameter->envelope, parameter_data)
	, tongue_index(parameters.tongue_index->envelope, parameter_data)
	, tongue_diameter(parameters.tongue_diameter->envelope, parameter_data)
	, fricative_intensity(parameters.fricative_intensity->envelope, parameter_data)
	, quality(parameters.quality->envelope, parameter_data)
{
}

AudioData::Sliders::Sliders(const Parameters::Sliders& parameters, const blink_ParameterData* parameter_data)
	: amp { parameters.amp->slider, parameter_data }
	, pan { parameters.pan->slider, parameter_data }
	, pitch { parameters.pitch->slider, parameter_data }
{
}

AudioData::AudioData(const Parameters& parameters, const blink_ParameterData* parameter_data)
	: envelopes { parameters.env, parameter_data }
	, sliders { parameters.sliders, parameter_data }
{
}

} // berk