#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace freeze {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(freeze::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(freeze::Parameters::Index::Env_Formant)> formant;
		blink::EnvelopeData<int(freeze::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(freeze::Parameters::Index::Slider_Pitch)> pitch;

		Sliders(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} sliders;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // freeze