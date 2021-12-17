#include "audio_data.h"

namespace fudge {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
	, pan(plugin, plugin->params().env.pan->envelope(), buffer->parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, speed(plugin, plugin->params().env.speed->envelope(), buffer->parameter_data)
	, grain_size(plugin, plugin->params().env.grain.size->envelope(), buffer->parameter_data)
	, grain_transpose(plugin, plugin->params().env.grain.transpose->envelope(), buffer->parameter_data)
	, grain_uniformity(plugin, plugin->params().env.grain.uniformity->envelope(), buffer->parameter_data)
	, harmonics_amount(plugin, plugin->params().env.harmonics.amount->envelope(), buffer->parameter_data)
	, harmonics_spread(plugin, plugin->params().env.harmonics.spread->envelope(), buffer->parameter_data)
	, noise_amount(plugin, plugin->params().env.noise.amount->envelope(), buffer->parameter_data)
	, noise_color(plugin, plugin->params().env.noise.color->envelope(), buffer->parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: amp(plugin, plugin->params().sliders.amp->slider(), buffer->parameter_data)
	, pan(plugin, plugin->params().sliders.pan->slider(), buffer->parameter_data)
	, pitch(plugin, plugin->params().sliders.pitch->slider(), buffer->parameter_data)
	, speed(plugin, plugin->params().sliders.speed->slider(), buffer->parameter_data)
	, noise_width(plugin, plugin->params().sliders.noise_width->slider(), buffer->parameter_data)
	, sample_offset(plugin->get_int_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_SampleOffset))->value)
{
}

AudioData::Chords::Chords(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: scale(plugin, buffer->parameter_data, plugin->params().chords.scale.get())
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: noise_mode(plugin->get_option_data(buffer->parameter_data, int(Parameters::Index::Option_NoiseMode))->data.points[0].y)
{
}

AudioData::Toggles::Toggles(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: loop(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Loop))->data.points[0].value == BLINK_TRUE)
	, reverse(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Reverse))->data.points[0].value == BLINK_TRUE)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: envelopes(plugin, buffer)
	, sliders(plugin, buffer)
	, chords(plugin, buffer)
	, options(plugin, buffer)
	, toggles(plugin, buffer)
	, warp_points(buffer->warp_points)
{
}

} // fudge