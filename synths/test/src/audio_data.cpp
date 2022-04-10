#include "audio_data.h"

namespace test {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: amp(plugin.params().env.amp->envelope, parameter_data)
	, pan(plugin.params().env.pan->envelope, parameter_data)
	, wave(plugin.params().env.carrier.wave->envelope, parameter_data)
	, p0(plugin.params().env.carrier.pitch->envelope, parameter_data)
	, p1(plugin.params().env.modulator.pitch->envelope, parameter_data)
	, fm0(plugin.params().env.carrier.fm->envelope, parameter_data)
	, fm1(plugin.params().env.modulator.fm->envelope, parameter_data)
	, noise_amount(plugin.params().env.noise_amount->envelope, parameter_data)
	, noise_color(plugin.params().env.noise_color->envelope, parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_width(plugin.params().sliders.noise_width->slider, parameter_data)
	, amp(plugin.params().sliders.amp->slider, parameter_data)
	, pan(plugin.params().sliders.pan->slider, parameter_data)
	, carrier_pitch(plugin.params().sliders.carrier_pitch->slider, parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: noise_mode { *plugin.params().options.noise_mode, parameter_data }
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, sliders(plugin, parameter_data)
	, options(plugin, parameter_data)
{
}

} // test