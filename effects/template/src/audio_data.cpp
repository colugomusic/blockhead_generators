#include "audio_data.h"

namespace compressor {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: example(plugin, buffer->parameter_data, plugin->params().env.example.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
{
}

} // compressor