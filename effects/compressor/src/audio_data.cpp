#include "audio_data.h"

namespace compressor {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: attack(plugin.params().env.attack->envelope, parameter_data)
	, release(plugin.params().env.release->envelope, parameter_data)
	, threshold(plugin.params().env.threshold->envelope, parameter_data)
	, ratio(plugin.params().env.ratio->envelope, parameter_data)
	, knee(plugin.params().env.knee->envelope, parameter_data)
	, mix(plugin.params().env.mix->envelope, parameter_data)
{
}

AudioData::Options::Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: stereo(*plugin.params().options.stereo.get(), parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
	, options(plugin, parameter_data)
{
}

} // compressor