#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin, plugin->params().env.pitch->envelope(), parameter_data)
	, feedback(plugin, plugin->params().env.feedback->envelope(), parameter_data)
	, damper(plugin, plugin->params().env.damper->envelope(), parameter_data)
	, fm_amount(plugin, plugin->params().env.fm.amount->envelope(), parameter_data)
	, fm_ratio(plugin, plugin->params().env.fm.ratio->envelope(), parameter_data)
	, harmonics_amount(plugin, plugin->params().env.harmonics.amount->envelope(), parameter_data)
	, harmonics_spread(plugin, plugin->params().env.harmonics.spread->envelope(), parameter_data)
	, harmonics_scale_snap_amount(plugin, plugin->params().env.harmonics.scale_snap_amount->envelope(), parameter_data)
	, width(plugin, plugin->params().env.harmonics.width->envelope(), parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), parameter_data)
{
}

AudioData::Chords::Chords(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: harmonics_scale(plugin, parameter_data, plugin->params().chords.harmonics.scale.get())
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin, plugin->params().sliders.pitch->slider(), parameter_data)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, chords(plugin, parameter_data)
	, sliders(plugin, parameter_data)
{
}

} // resonator