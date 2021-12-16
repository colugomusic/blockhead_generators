#pragma once

#include <blink/parameters/chord_spec.hpp>
#include <blink/envelope_spec.hpp>
#include <blink/parameters/slider_parameter_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace freeze {

struct Parameters
{
	enum class Index
	{
		Env_Pitch,
		Env_Formant,
		Env_Mix,
		Slider_Pitch,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> formant;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> pitch;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // freeze
