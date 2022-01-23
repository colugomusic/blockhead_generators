#include "audio_data.h"

namespace spring_reverb {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: size(plugin.params().env.size->envelope, parameter_data)
	, decay(plugin.params().env.decay->envelope, parameter_data)
	, mix(plugin.params().env.mix->envelope, parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // spring_reverb