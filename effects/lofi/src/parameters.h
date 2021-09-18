#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/plugin.hpp>

namespace lofi {

struct Parameters
{
	enum class Index
	{
		Env_SR,
		Env_BR,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> sr;
		std::shared_ptr<blink::EnvelopeParameter> br;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

} // lofi
