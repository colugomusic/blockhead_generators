#include "audio_data.h"

namespace saturator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: drive(plugin, buffer->parameter_data, plugin->params().env.drive.get())
	, amp(plugin, buffer->parameter_data, plugin->params().env.amp.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // saturator