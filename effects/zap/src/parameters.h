#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/plugin.hpp>

namespace zap {

struct Parameters
{
	enum class Index
	{
		Env_Frequency,
		Env_Resonance,
		Env_Spread,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> freq;
		std::shared_ptr<blink::EnvelopeParameter> res;
		std::shared_ptr<blink::EnvelopeParameter> spread;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

} // zap
