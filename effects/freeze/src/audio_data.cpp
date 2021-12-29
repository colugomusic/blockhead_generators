#include "audio_data.h"

namespace freeze {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin, plugin.params().env.pitch->envelope(), parameter_data)
	, formant(plugin, plugin.params().env.formant->envelope(), parameter_data)
	, mix(plugin, plugin.params().env.mix->envelope(), parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin, plugin.params().sliders.pitch->slider(), parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, sliders(plugin, parameter_data)
{
}

} // freeze