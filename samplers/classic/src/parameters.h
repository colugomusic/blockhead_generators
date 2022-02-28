#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/parameters/slider_parameter_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace classic {

struct Parameters
{
	enum class Index
	{
		Option_NoiseMode,
		Option_Reverse,
		Sld_NoiseWidth,

		Env_Amp,
		Env_Pan,
		Env_Pitch,
		Env_NoiseAmount,
		Env_NoiseColor,

		Sld_Amp,
		Sld_Pan,
		Sld_Pitch,
		Sld_SampleOffset,

		Tog_Loop,
		Tog_Reverse,
	};


	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> pan;
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		
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
		std::shared_ptr<blink::SliderParameter<int>> sample_offset;
		std::shared_ptr<blink::SliderParameter<float>> noise_width;
	} sliders;

	struct Options
	{
		std::shared_ptr<blink::OptionParameter> noise_mode;
		std::shared_ptr<blink::OptionParameter> reverse;
	} options;

	struct Toggles
	{
		std::shared_ptr<blink::ToggleParameter> loop;
		std::shared_ptr<blink::ToggleParameter> reverse;
	} toggles;
	
	Parameters(blink::Plugin* plugin);
};

} // classic
