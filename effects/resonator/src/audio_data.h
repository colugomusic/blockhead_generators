#pragma once

#include <blink/chord_data.hpp>
#include <blink/envelope_data.hpp>
#include <blink/option_data.hpp>
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
		blink::EnvelopeData<int(resonator::Parameters::Index::Env_Mix)> mix;

		Envelopes(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} envelopes;

	struct Chords
	{
		blink::ChordData<int(resonator::Parameters::Index::Chord_HarmonicsScale)> harmonics_scale;

		Chords(const Plugin* plugin, const blink_EffectBuffer* buffer);
	} chords;

	AudioData(const Plugin* plugin, const blink_EffectBuffer* buffer);
};

} // resonator