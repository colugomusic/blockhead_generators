#include "audio_data.h"

namespace compressor {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: attack(plugin, plugin->params().env.attack->envelope(), buffer->parameter_data)
	, release(plugin, plugin->params().env.release->envelope(), buffer->parameter_data)
	, threshold(plugin, plugin->params().env.threshold->envelope(), buffer->parameter_data)
	, ratio(plugin, plugin->params().env.ratio->envelope(), buffer->parameter_data)
	, knee(plugin, plugin->params().env.knee->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: stereo(plugin, buffer->parameter_data, plugin->params().options.stereo.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, options(plugin, buffer)
{
}

} // compressor