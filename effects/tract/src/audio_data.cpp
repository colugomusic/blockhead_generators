#include "audio_data.h"

namespace tract {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: index(plugin, plugin.params().env.index->envelope(), parameter_data)
	, diameter(plugin, plugin.params().env.diameter->envelope(), parameter_data)
	, tongue_index(plugin, plugin.params().env.tongue_index->envelope(), parameter_data)
	, tongue_diameter(plugin, plugin.params().env.tongue_diameter->envelope(), parameter_data)
	, quality(plugin, plugin.params().env.quality->envelope(), parameter_data)
	, mix(plugin, plugin.params().env.mix->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // tract