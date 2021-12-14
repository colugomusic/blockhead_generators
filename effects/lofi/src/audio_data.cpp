#include "audio_data.h"

namespace lofi {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: BR(plugin, plugin->params().env.br->envelope(), buffer->parameter_data)
	, SR(plugin, plugin->params().env.sr->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // lofi