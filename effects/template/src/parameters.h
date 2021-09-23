#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace template {

struct Parameters
{
	enum class Index
	{
		Env_Example,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> example;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;
	
	Parameters(blink::Plugin* plugin);
};

} // template
