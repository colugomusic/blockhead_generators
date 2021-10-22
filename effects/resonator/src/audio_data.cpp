#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, buffer->parameter_data, plugin->params().env.pitch.get())
	, dispersion(plugin, buffer->parameter_data, plugin->params().env.dispersion.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // resonator