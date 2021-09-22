#include "audio_data.h"

namespace wavebender {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: bubble(plugin, buffer->parameter_data, plugin->params().env.bubble.get())
	, tilt(plugin, buffer->parameter_data, plugin->params().env.tilt.get())
	, pitch(plugin, buffer->parameter_data, plugin->params().env.pitch.get())
	, xfade_size(plugin, buffer->parameter_data, plugin->params().env.xfade_size.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: xfade_mode(plugin, buffer->parameter_data, plugin->params().opt.xfade_mode.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, options(plugin, buffer)
{
}

} // wavebender