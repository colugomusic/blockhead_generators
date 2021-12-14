#include "audio_data.h"

namespace wavebender {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: tilt(plugin, plugin->params().env.tilt->envelope(), buffer->parameter_data)
	, xfade_size(plugin, plugin->params().env.xfade_size->envelope(), buffer->parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, bubble(plugin, plugin->params().env.bubble->envelope(), buffer->parameter_data)
	, smoother(plugin, plugin->params().env.smoother->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
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