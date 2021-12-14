#include "audio_data.h"

namespace berk {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, buzz(plugin, plugin->params().env.buzz->envelope(), buffer->parameter_data)
	, index(plugin, plugin->params().env.index->envelope(), buffer->parameter_data)
	, diameter(plugin, plugin->params().env.diameter->envelope(), buffer->parameter_data)
	, tongue_index(plugin, plugin->params().env.tongue_index->envelope(), buffer->parameter_data)
	, tongue_diameter(plugin, plugin->params().env.tongue_diameter->envelope(), buffer->parameter_data)
	, fricative_intensity(plugin, plugin->params().env.fricative_intensity->envelope(), buffer->parameter_data)
	, quality(plugin, plugin->params().env.quality->envelope(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // berk