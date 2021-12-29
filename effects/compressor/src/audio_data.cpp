#include "audio_data.h"

namespace compressor {

AudioData::Envelopes::Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: attack(plugin, plugin->params().env.attack->envelope(), parameter_data)
	, release(plugin, plugin->params().env.release->envelope(), parameter_data)
	, threshold(plugin, plugin->params().env.threshold->envelope(), parameter_data)
	, ratio(plugin, plugin->params().env.ratio->envelope(), parameter_data)
	, knee(plugin, plugin->params().env.knee->envelope(), parameter_data)
	, mix(plugin, plugin->params().env.mix->envelope(), parameter_data)
{
}

AudioData::Options::Options(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: stereo(plugin, parameter_data, plugin->params().options.stereo.get())
{
}

AudioData::AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, options(plugin, parameter_data)
{
}

} // compressor