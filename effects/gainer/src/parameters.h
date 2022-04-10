#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/slider_spec.hpp>
#include <blink/plugin.hpp>

namespace gainer {

struct Parameters
{
	enum class Index
	{
		Env_Amp,
		Env_Pan,
		Sld_Amp,
		Sld_Pan,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> pan;
	} env;

	struct Sliders
	{
		std::shared_ptr<blink::SliderParameter<float>> amp;
		std::shared_ptr<blink::SliderParameter<float>> pan;
	} sliders;
	
	Parameters(blink::Plugin* plugin);
};

} // gainer
