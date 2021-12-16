#include "audio_data.h"

namespace freeze {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, formant(plugin, plugin->params().env.formant->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, plugin->params().sliders.pitch->slider(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, sliders(plugin, buffer)
{
}

} // freeze