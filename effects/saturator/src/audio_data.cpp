#include "audio_data.h"

namespace saturator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: drive(plugin, plugin->params().env.drive->envelope(), buffer->parameter_data)
	, amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // saturator