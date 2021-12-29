#include "audio_data.h"

namespace classic {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin->params().env.amp->envelope(), parameter_data)
	, pan(plugin, plugin->params().env.pan->envelope(), parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), parameter_data)
	, noise_amount(plugin, plugin->params().env.noise.amount->envelope(), parameter_data)
	, noise_color(plugin, plugin->params().env.noise.color->envelope(), parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin->params().sliders.amp->slider(), parameter_data)
	, pan(plugin, plugin->params().sliders.pan->slider(), parameter_data)
	, pitch(plugin, plugin->params().sliders.pitch->slider(), parameter_data)
	, noise_width(plugin, plugin->params().sliders.noise_width->slider(), parameter_data)
	, sample_offset(plugin->get_int_slider_data(parameter_data, int(Parameters::Index::Sld_SampleOffset))->value)
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: noise_mode(plugin->get_option_data(parameter_data, int(Parameters::Index::Option_NoiseMode))->data.points[0].y)
{
}

AudioData::Toggles::Toggles(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: loop(plugin->get_toggle_data(parameter_data, int(Parameters::Index::Tog_Loop))->data.points[0].value == BLINK_TRUE)
	, reverse(plugin->get_toggle_data(parameter_data, int(Parameters::Index::Tog_Reverse))->data.points[0].value == BLINK_TRUE)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, sliders(plugin, parameter_data)
	, options(plugin, parameter_data)
	, toggles(plugin, parameter_data)
{
}

} // classic