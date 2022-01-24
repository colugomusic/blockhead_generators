#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/parameters/option_data.hpp>
#include "plugin.h"
#include "parameters.h"

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

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Sliders
	{
		Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data);

		blink::SliderData<int(classic::Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(classic::Parameters::Index::Sld_Pan)> pan;
		blink::SliderData<int(classic::Parameters::Index::Sld_Pitch)> pitch;
		blink::SliderData<int(classic::Parameters::Index::Sld_NoiseWidth)> noise_width;
		blink::IntSliderData<int(classic::Parameters::Index::Sld_SampleOffset)> sample_offset;
	} sliders;

	struct Options
	{
		Options(const Plugin& plugin, const blink_ParameterData* parameter_data);

		int noise_mode;
	} options;

	struct Toggles
	{
		Toggles(const Plugin& plugin, const blink_ParameterData* parameter_data);

		bool loop;
		bool reverse;
	} toggles;

	AudioData(const Plugin& plugin, const blink_ParameterData* parameter_data);
};

} // classic