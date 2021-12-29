#include "audio_data.h"

namespace berk {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin->params().env.amp->envelope(), parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), parameter_data)
	, buzz(plugin, plugin->params().env.buzz->envelope(), parameter_data)
	, index(plugin, plugin->params().env.index->envelope(), parameter_data)
	, diameter(plugin, plugin->params().env.diameter->envelope(), parameter_data)
	, tongue_index(plugin, plugin->params().env.tongue_index->envelope(), parameter_data)
	, tongue_diameter(plugin, plugin->params().env.tongue_diameter->envelope(), parameter_data)
	, fricative_intensity(plugin, plugin->params().env.fricative_intensity->envelope(), parameter_data)
	, quality(plugin, plugin->params().env.quality->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // berk