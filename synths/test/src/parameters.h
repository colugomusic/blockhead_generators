#pragma once

#include <blink/plugin.hpp>

namespace test {

struct Parameters
{
	enum class Index
	{
		Env_Amp,
		Env_Carrier_Wave,
		Env_Carrier_Pitch,
		Env_Carrier_FM,
		Env_Modulator_Pitch,
		Env_Modulator_FM,
		Option_NoiseMode,
		Env_NoiseAmount,
		Env_NoiseColor,
		Sld_NoiseWidth,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;

		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> wave;
			std::shared_ptr<blink::EnvelopeParameter> pitch;
			std::shared_ptr<blink::EnvelopeParameter> fm;
		} carrier;

		struct
		{
			std::shared_ptr<blink::EnvelopeParameter> pitch;
			std::shared_ptr<blink::EnvelopeParameter> fm;
		} modulator;

		std::shared_ptr<blink::EnvelopeParameter> noise_amount;
		std::shared_ptr<blink::EnvelopeParameter> noise_color;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Options
	{
		std::shared_ptr<blink::OptionParameter> noise_mode;
	} options;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> noise_width;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // test
