#include "audio_data.h"

namespace spring_reverb {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: size(plugin, plugin->params().env.size->envelope(), buffer->parameter_data)
	, decay(plugin, plugin->params().env.decay->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // spring_reverb