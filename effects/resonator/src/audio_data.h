#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/parameters/chord_data.hpp>
#include <blink/parameters/option_data.hpp>
#include <blink/effect_unit.hpp>
#include "plugin.h"

namespace resonator {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Feedback)> feedback;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Damper)> damper;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_FM_Amount)> fm_amount;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_FM_Ratio)> fm_ratio;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_HarmonicsAmount)> harmonics_amount;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_HarmonicsSpread)> harmonics_spread;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_HarmonicsScaleSnapAmount)> harmonics_scale_snap_amount;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_HarmonicsWidth)> width;
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Chords
	{
		blink::ChordData<int(resonator::Parameters::Index::Chord_HarmonicsScale)> harmonics_scale;

		Chords(const Plugin* plugin, const blink_ParameterData* parameter_data);
	} chords;

	struct Sliders
	{
		Sliders(const Plugin* plugin, const blink_ParameterData* parameter_data);

		blink::SliderData<int(resonator::Parameters::Index::Slider_Pitch)> pitch;
	} sliders;

	AudioData(const Plugin* plugin, const blink_ParameterData* parameter_data);
};

} // resonator