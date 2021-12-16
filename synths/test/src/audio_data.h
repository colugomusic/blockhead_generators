#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace test {

struct AudioData
{
	struct Envelopes
	{
		Envelopes(const Plugin* plugin, const blink_SynthBuffer* buffer);

		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Wave)> wave;
		blink::EnvelopeData<int(Parameters::Index::Env_Pitch0)> p0;
		blink::EnvelopeData<int(Parameters::Index::Env_Pitch1)> p1;
		blink::EnvelopeData<int(Parameters::Index::Env_FM0)> fm0;
		blink::EnvelopeData<int(Parameters::Index::Env_FM1)> fm1;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseAmount)> noise_amount;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseColor)> noise_color;
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(Parameters::Index::Sld_NoiseWidth)> noise_width;

		Sliders(const Plugin* plugin, const blink_SynthBuffer* buffer);
	} sliders;

	struct Options
	{
		Options(const Plugin* plugin, const blink_SynthBuffer* buffer);

		int noise_mode;
	} options;

	AudioData(const Plugin* plugin, const blink_SynthBuffer* buffer);
};

} // test