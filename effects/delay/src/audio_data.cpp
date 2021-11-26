#include "audio_data.h"

namespace delay {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: time(plugin, buffer->parameter_data, plugin->params().env.time.get())
	, feedback(plugin, buffer->parameter_data, plugin->params().env.feedback.get())
	, dampener(plugin, buffer->parameter_data, plugin->params().env.dampener.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // delay