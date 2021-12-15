#include "audio_data.h"

namespace classic {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
	, pan(plugin, plugin->params().env.pan->envelope(), buffer->parameter_data)
	, pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
	, noise_amount(plugin, plugin->params().env.noise.amount->envelope(), buffer->parameter_data)
	, noise_color(plugin, plugin->params().env.noise.color->envelope(), buffer->parameter_data)
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: amp(plugin->get_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_Amp))->value)
	, pan(plugin->get_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_Pan))->value)
	, pitch(plugin->get_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_Pitch))->value)
	, sample_offset(plugin->get_int_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_SampleOffset))->value)
	, noise_width(plugin->get_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_NoiseWidth))->value)
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: noise_mode(plugin->get_option_data(buffer->parameter_data, int(Parameters::Index::Option_NoiseMode))->index)
{
}

AudioData::Toggles::Toggles(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: loop(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Loop))->value)
	, reverse(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Reverse))->value)
{
}

AudioData::Manipulators::Manipulators(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: amp(buffer->manipulator_target_data[int(ManipulatorTargets::Index::Slider_Amp)])
	, pan(buffer->manipulator_target_data[int(ManipulatorTargets::Index::Slider_Pan)])
	, pitch(buffer->manipulator_target_data[int(ManipulatorTargets::Index::Slider_Pitch)])
{
}

AudioData::AudioData(const Plugin* plugin, const blink_SamplerBuffer* buffer)
	: envelopes(plugin, buffer)
	, sliders(plugin, buffer)
	, options(plugin, buffer)
	, toggles(plugin, buffer)
	, manipulators(plugin, buffer)
{
}

} // classic