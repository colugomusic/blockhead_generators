#pragma once

#include <blink/envelope_data.hpp>
#include <blink/parameters/option_data.hpp>
#include "plugin.h"

namespace classic {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(classic::Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(classic::Parameters::Index::Env_Pan)> pan;
		blink::EnvelopeData<int(classic::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(classic::Parameters::Index::Env_NoiseAmount)> noise_amount;
		blink::EnvelopeData<int(classic::Parameters::Index::Env_NoiseColor)> noise_color;

		Envelopes(const Plugin* plugin, const blink_SamplerBuffer* buffer);
	} envelopes;

	struct Sliders
	{
		Sliders(const Plugin* plugin, const blink_SamplerBuffer* buffer);

		float amp;
		float pan;
		float pitch;
		int sample_offset;
		float noise_width;
	} sliders;

	struct Options
	{
		Options(const Plugin* plugin, const blink_SamplerBuffer* buffer);

		int noise_mode;
	} options;

	struct Toggles
	{
		Toggles(const Plugin* plugin, const blink_SamplerBuffer* buffer);

		bool loop;
		bool reverse;
	} toggles;

	struct Manipulators
	{
	};

	AudioData(const Plugin* plugin, const blink_SamplerBuffer* buffer);
};

} // classic