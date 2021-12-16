#include "audio_data.h"

namespace test {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
	, wave(plugin, plugin->params().env.wave->envelope(), buffer->parameter_data)
	, p0(plugin, plugin->params().env.p0->envelope(), buffer->parameter_data)
	, p1(plugin, plugin->params().env.p1->envelope(), buffer->parameter_data)
	, fm0(plugin, plugin->params().env.fm0->envelope(), buffer->parameter_data)
	, fm1(plugin, plugin->params().env.fm1->envelope(), buffer->parameter_data)
	, noise_amount(plugin, plugin->params().env.noise_amount->envelope(), buffer->parameter_data)
	, noise_color(plugin, plugin->params().env.noise_color->envelope(), buffer->parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: noise_width(plugin, plugin->params().sliders.noise_width->slider(), buffer->parameter_data)
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: noise_mode(plugin->get_option_data(buffer->parameter_data, int(Parameters::Index::Option_NoiseMode))->data.points[0].y)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_SynthBuffer* buffer)
	: envelopes(plugin, buffer)
	, sliders(plugin, buffer)
	, options(plugin, buffer)
{
}

} // test