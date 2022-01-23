#include "audio_data.h"

namespace lowpass {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: freq(plugin.params().env.freq->envelope, parameter_data)
	, res(plugin.params().env.res->envelope, parameter_data)
	, mix(plugin.params().env.mix->envelope, parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // lowpass