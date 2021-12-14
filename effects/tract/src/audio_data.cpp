#include "audio_data.h"

namespace tract {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: index(plugin, plugin->params().env.index->envelope(), buffer->parameter_data)
	, diameter(plugin, plugin->params().env.diameter->envelope(), buffer->parameter_data)
	, tongue_index(plugin, plugin->params().env.tongue_index->envelope(), buffer->parameter_data)
	, tongue_diameter(plugin, plugin->params().env.tongue_diameter->envelope(), buffer->parameter_data)
	, quality(plugin, plugin->params().env.quality->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // tract