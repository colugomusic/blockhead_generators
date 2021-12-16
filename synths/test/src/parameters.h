#pragma once

#include <blink/plugin.hpp>

namespace test {

struct Parameters
{
	enum class Index
	{
		Env_Amp,
		Env_Wave,
		Env_Pitch0,
		Env_Pitch1,
		Env_FM0,
		Env_FM1,
		Option_NoiseMode,
		Env_NoiseAmount,
		Env_NoiseColor,
		Sld_NoiseWidth,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> wave;
		std::shared_ptr<blink::EnvelopeParameter> p0;
		std::shared_ptr<blink::EnvelopeParameter> p1;
		std::shared_ptr<blink::EnvelopeParameter> fm0;
		std::shared_ptr<blink::EnvelopeParameter> fm1;
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
