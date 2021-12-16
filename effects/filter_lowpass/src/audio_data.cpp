#include "audio_data.h"

namespace lowpass {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: freq(plugin, plugin->params().env.freq->envelope(), buffer->parameter_data)
	, res(plugin, plugin->params().env.res->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // lowpass