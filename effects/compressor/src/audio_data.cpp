#include "audio_data.h"

namespace compressor {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: attack(plugin, buffer->parameter_data, plugin->params().env.attack.get())
	, release(plugin, buffer->parameter_data, plugin->params().env.release.get())
	, threshold(plugin, buffer->parameter_data, plugin->params().env.threshold.get())
	, ratio(plugin, buffer->parameter_data, plugin->params().env.ratio.get())
	, knee(plugin, buffer->parameter_data, plugin->params().env.knee.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
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