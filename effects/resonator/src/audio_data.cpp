#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, feedback(plugin, plugin->params().env.feedback->envelope(), buffer->parameter_data)
	, damper(plugin, plugin->params().env.damper->envelope(), buffer->parameter_data)
	, fm_amount(plugin, plugin->params().env.fm.amount->envelope(), buffer->parameter_data)
	, fm_ratio(plugin, plugin->params().env.fm.ratio->envelope(), buffer->parameter_data)
	, harmonics_amount(plugin, plugin->params().env.harmonics.amount->envelope(), buffer->parameter_data)
	, harmonics_spread(plugin, plugin->params().env.harmonics.spread->envelope(), buffer->parameter_data)
	, harmonics_scale_snap_amount(plugin, plugin->params().env.harmonics.scale_snap_amount->envelope(), buffer->parameter_data)
	, width(plugin, plugin->params().env.harmonics.width->envelope(), buffer->parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), buffer->parameter_data)
{
}

AudioData::Chords::Chords(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: harmonics_scale(plugin, buffer->parameter_data, plugin->params().chords.harmonics.scale.get())
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, plugin->params().sliders.pitch->slider(), buffer->parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, chords(plugin, buffer)
	, sliders(plugin, buffer)
{
}

} // resonator