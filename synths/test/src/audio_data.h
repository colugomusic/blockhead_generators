#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/effect_unit.hpp>
#include <blink/parameters/option_data.hpp>
#include "plugin.h"

namespace test {

struct AudioData
{
	struct Envelopes
	{
		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);

		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Pan)> pan;
		blink::EnvelopeData<int(Parameters::Index::Env_Carrier_Wave)> wave;
		blink::EnvelopeData<int(Parameters::Index::Env_Carrier_Pitch)> p0;
		blink::EnvelopeData<int(Parameters::Index::Env_Carrier_FM)> fm0;
		blink::EnvelopeData<int(Parameters::Index::Env_Modulator_Pitch)> p1;
		blink::EnvelopeData<int(Parameters::Index::Env_Modulator_FM)> fm1;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseAmount)> noise_amount;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseColor)> noise_color;
	} envelopes;

	struct Sliders
	{
		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Pan)> pan;
		blink::SliderData<int(Parameters::Index::Sld_Carrier_Pitch)> carrier_pitch;
		blink::SliderData<int(Parameters::Index::Sld_NoiseWidth)> noise_width;

		Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} sliders;

	struct Options
	{
		Options(const Plugin& plugin, const blink_ParameterData* parameter_data);

		blink::OptionData<int(Parameters::Index::Option_NoiseMode)> noise_mode;
	} options;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // test