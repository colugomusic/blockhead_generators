#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/slider_spec.hpp>
#include <blink/plugin.hpp>

namespace rattler {

struct Parameters
{
	enum class Index
	{
		Env_Amp,
		Env_Time,
		Env_Feedback,
		Env_Width,
		Sld_Amp,
		Sld_Width,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> time;
		std::shared_ptr<blink::EnvelopeParameter> feedback;
		std::shared_ptr<blink::EnvelopeParameter> width;
	} env;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> amp;
		std::shared_ptr<blink::SliderParameter<float>> width;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // rattler
