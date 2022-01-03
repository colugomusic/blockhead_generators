#include "audio_data.h"

namespace ring_modulator {

AudioData::Envelopes::Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: pitch(plugin.params().env.pitch->envelope(), parameter_data)
	, mix(plugin.params().env.mix->envelope(), parameter_data)
{
}

AudioData::AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data)
	: envelopes(plugin, parameter_data)
{
}

} // ring_modulator