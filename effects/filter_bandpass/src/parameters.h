#pragma once

#include <blink/plugin.hpp>

namespace bandpass {

struct Parameters
{
	enum class Index
	{
		Env_Freq,
		Env_Res,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> freq;
		std::shared_ptr<blink::EnvelopeParameter> res;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

} // bandpass
