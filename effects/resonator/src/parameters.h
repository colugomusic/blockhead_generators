#pragma once

#include <blink/chord_spec.hpp>
#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace resonator {

struct Parameters
{
	enum class Index
	{
		Env_Pitch,
		Env_Feedback,
		Env_Damper,
		Chord_HarmonicsScale,
		Env_HarmonicsAmount,
		Env_HarmonicsSpread,
		Env_HarmonicsScaleSnapAmount,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> feedback;
		std::shared_ptr<blink::EnvelopeParameter> damper;
		std::shared_ptr<blink::EnvelopeParameter> harmonics_amount;
		std::shared_ptr<blink::EnvelopeParameter> harmonics_spread;
		std::shared_ptr<blink::EnvelopeParameter> harmonics_scale_snap_amount;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Chords
	{
		std::shared_ptr<blink::ChordParameter> harmonics_scale;
	} chords;
	
	Parameters(blink::Plugin* plugin);
};

} // resonator
