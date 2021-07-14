#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/plugin.hpp>

namespace spring_reverb {

struct Parameters
{
	enum class Index
	{
		Env_Size,
		Env_Decay,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> size;
		std::shared_ptr<blink::EnvelopeParameter> decay;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

} // spring_reverb