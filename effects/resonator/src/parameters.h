#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace resonator {

struct Parameters
{
	enum class Index
	{
		Env_Pitch,
		Env_Dispersion,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> dispersion;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;
	
	Parameters(blink::Plugin* plugin);
};

} // resonator
