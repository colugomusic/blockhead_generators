#include "audio_data.h"

namespace saturator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: drive(plugin, plugin->params().env.drive->envelope(), parameter_data)
	, amp(plugin, plugin->params().env.amp->envelope(), parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // saturator