#pragma once

#include <blink/parameters/chord_spec.hpp>
#include <blink/envelope_spec.hpp>
#include <blink/parameters/slider_parameter_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace resonator {

struct Parameters
{
	enum class Index
	{
		Env_Pitch,
		Env_Feedback,
		Env_Damper,
		Env_FM_Amount,
		Env_FM_Ratio,
		Chord_HarmonicsScale,
		Env_HarmonicsAmount,
		Env_HarmonicsSpread,
		Env_HarmonicsScaleSnapAmount,
		Env_HarmonicsWidth,
		Env_Mix,
		Slider_Pitch,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> feedback;
		std::shared_ptr<blink::EnvelopeParameter> damper;
		
		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> amount;
			std::shared_ptr<blink::EnvelopeParameter> ratio;
		} fm;

		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> amount;
			std::shared_ptr<blink::EnvelopeParameter> spread;
			std::shared_ptr<blink::EnvelopeParameter> scale_snap_amount;
			std::shared_ptr<blink::EnvelopeParameter> width;
		} harmonics;

		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Chords
	{
		struct
		{
			std::shared_ptr<blink::ChordParameter> scale;
		} harmonics;
	} chords;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> pitch;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // resonator
