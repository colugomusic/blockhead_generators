#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/plugin.hpp>

namespace saturator {

struct Parameters
{
	enum class Index
	{
		Env_Drive,
		Env_Amp,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> drive;
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

} // saturator
