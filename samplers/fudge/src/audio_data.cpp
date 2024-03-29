#include "audio_data.h"

namespace fudge {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin.params.env.amp->envelope, parameter_data)
	, pan(plugin.params.env.pan->envelope, parameter_data)
	, pitch(plugin.params.env.pitch->envelope, parameter_data)
	, speed(plugin.params.env.speed->envelope, parameter_data)
	, grain_size(plugin.params.env.grain.size->envelope, parameter_data)
	, grain_transpose(plugin.params.env.grain.transpose->envelope, parameter_data)
	, grain_uniformity(plugin.params.env.grain.uniformity->envelope, parameter_data)
	, harmonics_amount(plugin.params.env.harmonics.amount->envelope, parameter_data)
	, harmonics_spread(plugin.params.env.harmonics.spread->envelope, parameter_data)
	, noise_amount(plugin.params.env.noise.amount->envelope, parameter_data)
	, noise_color(plugin.params.env.noise.color->envelope, parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin.params.sliders.amp->slider, parameter_data)
	, pan(plugin.params.sliders.pan->slider, parameter_data)
	, pitch(plugin.params.sliders.pitch->slider, parameter_data)
	, speed(plugin.params.sliders.speed->slider, parameter_data)
	, noise_width(plugin.params.sliders.noise_width->slider, parameter_data)
	, sample_offset(plugin.params.sliders.sample_offset->slider, parameter_data)
{
}

AudioData::Chords::Chords(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: scale(*plugin.params.chords.scale.get(), parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_mode { *plugin.params.options.noise_mode, parameter_data }
	, reverse_mode { *plugin.params.options.reverse_mode, parameter_data }
{
}

AudioData::Toggles::Toggles(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: loop { *plugin.params.toggles.loop, parameter_data }
	, reverse { *plugin.params.toggles.reverse, parameter_data }
{
}

AudioData::AudioData(const Plugin& plugin, const blink_SamplerUnitState& unit_state)
	: envelopes(plugin, unit_state.parameter_data)
	, sliders(plugin, unit_state.parameter_data)
	, chords(plugin, unit_state.parameter_data)
	, options(plugin, unit_state.parameter_data)
	, toggles(plugin, unit_state.parameter_data)
	, warp_points(unit_state.warp_points)
{
}

} // fudge