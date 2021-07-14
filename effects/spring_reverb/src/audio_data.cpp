#include "audio_data.h"

namespace spring_reverb {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: size(plugin, buffer->parameter_data, plugin->params().env.size.get())
	, decay(plugin, buffer->parameter_data, plugin->params().env.decay.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // spring_reverb