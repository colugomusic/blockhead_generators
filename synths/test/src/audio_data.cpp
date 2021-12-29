#include "audio_data.h"

namespace test {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin, plugin.params().env.amp->envelope(), parameter_data)
	, wave(plugin, plugin.params().env.wave->envelope(), parameter_data)
	, p0(plugin, plugin.params().env.p0->envelope(), parameter_data)
	, p1(plugin, plugin.params().env.p1->envelope(), parameter_data)
	, fm0(plugin, plugin.params().env.fm0->envelope(), parameter_data)
	, fm1(plugin, plugin.params().env.fm1->envelope(), parameter_data)
	, noise_amount(plugin, plugin.params().env.noise_amount->envelope(), parameter_data)
	, noise_color(plugin, plugin.params().env.noise_color->envelope(), parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_width(plugin, plugin.params().sliders.noise_width->slider(), parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_mode(plugin.get_option_data(parameter_data, int(Parameters::Index::Option_NoiseMode)).data.points[0].y)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, sliders(plugin, parameter_data)
	, options(plugin, parameter_data)
{
}

} // test