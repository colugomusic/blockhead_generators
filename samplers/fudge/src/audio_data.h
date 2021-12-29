#pragma once

#include <blink/envelope_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/parameters/chord_data.hpp>
#include <blink/parameters/option_data.hpp>
#include "plugin.h"
#include "parameters.h"

namespace fudge {

struct AudioData
{
	struct Envelopes
	{
		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Pan)> pan;
		blink::EnvelopeData<int(Parameters::Index::Env_Pitch)> pitch;
		blink::EnvelopeData<int(Parameters::Index::Env_Speed)> speed;
		blink::EnvelopeData<int(Parameters::Index::Env_GrainSize)> grain_size;
		blink::EnvelopeData<int(Parameters::Index::Env_GrainTranspose)> grain_transpose;
		blink::EnvelopeData<int(Parameters::Index::Env_Uniformity)> grain_uniformity;
		blink::EnvelopeData<int(Parameters::Index::Env_Harmonics_Amount)> harmonics_amount;
		blink::EnvelopeData<int(Parameters::Index::Env_Harmonics_Spread)> harmonics_spread;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseAmount)> noise_amount;
		blink::EnvelopeData<int(Parameters::Index::Env_NoiseColor)> noise_color;

		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data);
	} envelopes;

	struct Sliders
	{
		Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data);

		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Pan)> pan;
		blink::SliderData<int(Parameters::Index::Sld_Pitch)> pitch;
		blink::SliderData<int(Parameters::Index::Sld_Speed)> speed;
		blink::SliderData<int(Parameters::Index::Sld_NoiseWidth)> noise_width;

		int sample_offset;
	} sliders;

	struct Chords
	{
		Chords(const Plugin& plugin, const blink_ParameterData* parameter_data);

		blink::ChordData<int(Parameters::Index::Chord_Harmonics_Scale)> scale;
	} chords;

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

	const blink_WarpPoints* warp_points;

	AudioData(const Plugin& plugin, const blink_SamplerUnitState& unit_state);
};

} // fudge