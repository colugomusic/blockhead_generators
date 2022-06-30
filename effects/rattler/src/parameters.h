#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/slider_spec.hpp>
#include <blink/plugin.hpp>

namespace rattler {

struct Parameters
{
	enum class Index
	{
		Env_Time,
		Env_Feedback,
		Env_Width,
		Env_Dry,
		Env_Wet,
		Sld_Dry,
		Sld_Wet,
		Sld_Width,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> time;
		std::shared_ptr<blink::EnvelopeParameter> feedback;
		std::shared_ptr<blink::EnvelopeParameter> width;
		std::shared_ptr<blink::EnvelopeParameter> dry;
		std::shared_ptr<blink::EnvelopeParameter> wet;
	} env;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> dry;
		std::shared_ptr<blink::SliderParameter<float>> wet;
		std::shared_ptr<blink::SliderParameter<float>> width;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // rattler
