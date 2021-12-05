#include "audio_data.h"

namespace template {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: example(plugin, buffer->parameter_data, plugin->params().env.example.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // template