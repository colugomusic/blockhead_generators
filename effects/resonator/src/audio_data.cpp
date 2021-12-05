#include "audio_data.h"

namespace resonator {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin, buffer->parameter_data, plugin->params().env.pitch.get())
	, feedback(plugin, buffer->parameter_data, plugin->params().env.feedback.get())
	, damper(plugin, buffer->parameter_data, plugin->params().env.damper.get())
	, fm_amount(plugin, buffer->parameter_data, plugin->params().env.fm.amount.get())
	, fm_ratio(plugin, buffer->parameter_data, plugin->params().env.fm.ratio.get())
	, harmonics_amount(plugin, buffer->parameter_data, plugin->params().env.harmonics.amount.get())
	, harmonics_spread(plugin, buffer->parameter_data, plugin->params().env.harmonics.spread.get())
	, harmonics_scale_snap_amount(plugin, buffer->parameter_data, plugin->params().env.harmonics.scale_snap_amount.get())
	, width(plugin, buffer->parameter_data, plugin->params().env.harmonics.width.get())
	, mix(plugin, buffer->parameter_data, plugin->params().env.mix.get())
{
}

AudioData::Chords::Chords(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: harmonics_scale(plugin, buffer->parameter_data, plugin->params().chords.harmonics.scale.get())
{
}

AudioData::Sliders::Sliders(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: pitch(plugin->get_slider_data(buffer->parameter_data, int(Parameters::Index::Slider_Pitch))->value)
{
}

AudioData::AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer)
	: envelopes(plugin, buffer)
	, chords(plugin, buffer)
	, sliders(plugin, buffer)
{
}

} // resonator