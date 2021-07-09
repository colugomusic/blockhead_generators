#include "audio_data.h"

namespace tract {

AudioData::Envelopes::Envelopes(const Tract* plugin, const blink_EffectBuffer* buffer)
	: index(plugin, buffer->parameter_data, plugin->params().env.index.get())
	, diameter(plugin, buffer->parameter_data, plugin->params().env.diameter.get())
	, tongue_index(plugin, buffer->parameter_data, plugin->params().env.tongue_index.get())
	, tongue_diameter(plugin, buffer->parameter_data, plugin->params().env.tongue_diameter.get())
	, fricative_intensity(plugin, buffer->parameter_data, plugin->params().env.fricative_intensity.get())
	, quality(plugin, buffer->parameter_data, plugin->params().env.quality.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::AudioData(const Tract* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // tract