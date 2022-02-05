#include "audio_data.h"

namespace classic {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin.params.env.amp->envelope, parameter_data)
	, pan(plugin.params.env.pan->envelope, parameter_data)
	, pitch(plugin.params.env.pitch->envelope, parameter_data)
	, noise_amount(plugin.params.env.noise.amount->envelope, parameter_data)
	, noise_color(plugin.params.env.noise.color->envelope, parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin.params.sliders.amp->slider, parameter_data)
	, pan(plugin.params.sliders.pan->slider, parameter_data)
	, pitch(plugin.params.sliders.pitch->slider, parameter_data)
	, noise_width(plugin.params.sliders.noise_width->slider, parameter_data)
	, sample_offset(plugin.params.sliders.sample_offset->slider, parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_mode { *plugin.params.options.noise_mode, parameter_data }
{
}

AudioData::Toggles::Toggles(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: loop { *plugin.params.toggles.loop, parameter_data }
	, reverse { *plugin.params.toggles.reverse, parameter_data }
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, sliders(plugin, parameter_data)
	, options(plugin, parameter_data)
	, toggles(plugin, parameter_data)
{
}

} // classic