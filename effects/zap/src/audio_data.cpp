#include "audio_data.h"

namespace zap {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: frequency(plugin, buffer->parameter_data, plugin->params().env.freq.get())
	, resonance(plugin, buffer->parameter_data, plugin->params().env.res.get())
	, spread(plugin, buffer->parameter_data, plugin->params().env.spread.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // zap