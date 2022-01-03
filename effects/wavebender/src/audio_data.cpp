#include "audio_data.h"

namespace wavebender {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: tilt(plugin.params().env.tilt->envelope(), parameter_data)
	, xfade_size(plugin.params().env.xfade_size->envelope(), parameter_data)
	, pitch(plugin.params().env.pitch->envelope(), parameter_data)
	, bubble(plugin.params().env.bubble->envelope(), parameter_data)
	, smoother(plugin.params().env.smoother->envelope(), parameter_data)
	, mix(plugin.params().env.mix->envelope(), parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: xfade_mode(parameter_data, *plugin.params().opt.xfade_mode.get())
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, options(plugin, parameter_data)
{
}

} // wavebender