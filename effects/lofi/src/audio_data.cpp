#include "audio_data.h"

namespace lofi {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: BR(plugin, buffer->parameter_data, plugin->params().env.br.get())
	, SR(plugin, buffer->parameter_data, plugin->params().env.sr.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // lofi