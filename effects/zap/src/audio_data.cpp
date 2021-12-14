#include "audio_data.h"

namespace zap {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: frequency(plugin, plugin->params().env.freq->envelope(), buffer->parameter_data)
	, resonance(plugin, plugin->params().env.res->envelope(), buffer->parameter_data)
	, spread(plugin, plugin->params().env.spread->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // zap