#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, buffer->parameter_data, plugin->params().env.pitch.get())
	, feedback(plugin, buffer->parameter_data, plugin->params().env.feedback.get())
	, damper(plugin, buffer->parameter_data, plugin->params().env.damper.get())
	, harmonics_amount(plugin, buffer->parameter_data, plugin->params().env.harmonics_amount.get())
	, harmonics_spread(plugin, buffer->parameter_data, plugin->params().env.harmonics_spread.get())
	, harmonics_scale_snap_amount(plugin, buffer->parameter_data, plugin->params().env.harmonics_scale_snap_amount.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::Chords::Chords(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: harmonics_scale(plugin, buffer->parameter_data, plugin->params().chords.harmonics_scale.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, chords(plugin, buffer)
{
}

} // resonator