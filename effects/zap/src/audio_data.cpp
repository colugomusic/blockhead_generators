#include "audio_data.h"

namespace zap {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: frequency(plugin, plugin->params().env.freq->envelope(), parameter_data)
	, resonance(plugin, plugin->params().env.res->envelope(), parameter_data)
	, spread(plugin, plugin->params().env.spread->envelope(), parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // zap