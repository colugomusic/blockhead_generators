#include "audio_data.h"

namespace lofi {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: BR(plugin, plugin->params().env.br->envelope(), parameter_data)
	, SR(plugin, plugin->params().env.sr->envelope(), parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // lofi