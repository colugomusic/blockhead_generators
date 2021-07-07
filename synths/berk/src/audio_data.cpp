#include "audio_data.h"

namespace berk {

AudioData::Envelopes::Envelopes(const Berk* plugin, const blink_SynthBuffer* buffer)
	: amp(plugin, buffer->parameter_data, plugin->params().env.amp.get())
	, pitch(plugin, buffer->parameter_data, plugin->params().env.pitch.get())
	, buzz(plugin, buffer->parameter_data, plugin->params().env.buzz.get())
	, index(plugin, buffer->parameter_data, plugin->params().env.index.get())
	, diameter(plugin, buffer->parameter_data, plugin->params().env.diameter.get())
	, tongue_index(plugin, buffer->parameter_data, plugin->params().env.tongue_index.get())
	, tongue_diameter(plugin, buffer->parameter_data, plugin->params().env.tongue_diameter.get())
	, fricative_intensity(plugin, buffer->parameter_data, plugin->params().env.fricative_intensity.get())
	, quality(plugin, buffer->parameter_data, plugin->params().env.quality.get())
{
}

AudioData::AudioData(const Berk* plugin, const blink_SynthBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // berk