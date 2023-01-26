#pragma once

#include <blink/parameters/chord_parameter.hpp>
#include <blink/parameters/envelope_parameter.hpp>
#include <blink/parameters/slider_parameter.hpp>
#include <blink/plugin.hpp>

namespace fudge {

struct Parameters
{
	enum class Index
	{
		Option_NoiseMode,
		Option_ReverseMode,
		Sld_NoiseWidth,

		Env_Amp,
		Env_Pan,
		Env_Pitch,
		Env_Speed,
		Env_GrainSize,
		Env_GrainTranspose,
		Env_Uniformity,
		Chord_Harmonics_Scale,
		Env_Harmonics_Amount,
		Env_Harmonics_Spread,
		Env_NoiseAmount,
		Env_NoiseColor,

		Sld_Amp,
		Sld_Pan,
		Sld_Pitch,
		Sld_Speed,
		Sld_SampleOffset,

		Tog_Loop,
		Tog_Reverse,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> pan;
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> speed;
		
		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> size;
			std::shared_ptr<blink::EnvelopeParameter> transpose;
			std::shared_ptr<blink::EnvelopeParameter> uniformity;
		} grain;

		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> amount;
			std::shared_ptr<blink::EnvelopeParameter> spread;
		} harmonics;

		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> amount;
			std::shared_ptr<blink::EnvelopeParameter> color;
		} noise;
	} env;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> amp;
		std::shared_ptr<blink::SliderParameter<float>> pan;
		std::shared_ptr<blink::SliderParameter<float>> pitch;
		std::shared_ptr<blink::SliderParameter<float>> speed;
		std::shared_ptr<blink::SliderParameter<int>> sample_offset;
		std::shared_ptr<blink::SliderParameter<float>> noise_width;
	} sliders;

	struct Chords
	{
		std::shared_ptr<blink::ChordParameter> scale;
	} chords;

	struct Options
	{
		std::shared_ptr<blink::OptionParameter> noise_mode;
		std::shared_ptr<blink::OptionParameter> reverse_mode;
	} options;
	
	struct Toggles
	{
		std::shared_ptr<blink::OptionParameter> loop;
		std::shared_ptr<blink::OptionParameter> reverse;
	} toggles;
	
	Parameters(blink::Plugin* plugin);
};

} // fudge