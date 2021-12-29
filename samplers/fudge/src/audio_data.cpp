#include "audio_data.h"

namespace fudge {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin.params().env.amp->envelope(), parameter_data)
	, pan(plugin, plugin.params().env.pan->envelope(), parameter_data)
	, pitch(plugin, plugin.params().env.pitch->envelope(), parameter_data)
	, speed(plugin, plugin.params().env.speed->envelope(), parameter_data)
	, grain_size(plugin, plugin.params().env.grain.size->envelope(), parameter_data)
	, grain_transpose(plugin, plugin.params().env.grain.transpose->envelope(), parameter_data)
	, grain_uniformity(plugin, plugin.params().env.grain.uniformity->envelope(), parameter_data)
	, harmonics_amount(plugin, plugin.params().env.harmonics.amount->envelope(), parameter_data)
	, harmonics_spread(plugin, plugin.params().env.harmonics.spread->envelope(), parameter_data)
	, noise_amount(plugin, plugin.params().env.noise.amount->envelope(), parameter_data)
	, noise_color(plugin, plugin.params().env.noise.color->envelope(), parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin.params().sliders.amp->slider(), parameter_data)
	, pan(plugin, plugin.params().sliders.pan->slider(), parameter_data)
	, pitch(plugin, plugin.params().sliders.pitch->slider(), parameter_data)
	, speed(plugin, plugin.params().sliders.speed->slider(), parameter_data)
	, noise_width(plugin, plugin.params().sliders.noise_width->slider(), parameter_data)
	, sample_offset(plugin.get_int_slider_data(parameter_data, int(Parameters::Index::Sld_SampleOffset)).value)
{
}

AudioData::Chords::Chords(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: scale(plugin, parameter_data, plugin.params().chords.scale.get())
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_mode(plugin.get_option_data(parameter_data, int(Parameters::Index::Option_NoiseMode)).data.points[0].y)
{
}

AudioData::Toggles::Toggles(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: loop(plugin.get_toggle_data(parameter_data, int(Parameters::Index::Tog_Loop)).data.points[0].value == BLINK_TRUE)
	, reverse(plugin.get_toggle_data(parameter_data, int(Parameters::Index::Tog_Reverse)).data.points[0].value == BLINK_TRUE)
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