#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin.params().env.pitch->envelope, parameter_data)
	, feedback(plugin.params().env.feedback->envelope, parameter_data)
	, damper(plugin.params().env.damper->envelope, parameter_data)
	, fm_amount(plugin.params().env.fm.amount->envelope, parameter_data)
	, fm_ratio(plugin.params().env.fm.ratio->envelope, parameter_data)
	, harmonics_amount(plugin.params().env.harmonics.amount->envelope, parameter_data)
	, harmonics_spread(plugin.params().env.harmonics.spread->envelope, parameter_data)
	, harmonics_scale_snap_amount(plugin.params().env.harmonics.scale_snap_amount->envelope, parameter_data)
	, width(plugin.params().env.harmonics.width->envelope, parameter_data)
	, mix(plugin.params().env.mix->envelope, parameter_data)
{
}

AudioData::Chords::Chords(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: harmonics_scale(*plugin.params().chords.harmonics.scale.get(), parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin.params().sliders.pitch->slider, parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, chords(plugin, parameter_data)
	, sliders(plugin, parameter_data)
{
}

} // resonator