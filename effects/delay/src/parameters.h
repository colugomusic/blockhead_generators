#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace delay {

struct Parameters
{
	enum class Index
	{
		Env_Time,
		Env_Feedback,
		Env_Dampener,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> time;
		std::shared_ptr<blink::EnvelopeParameter> feedback;
		std::shared_ptr<blink::EnvelopeParameter> dampener;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;
	
	Parameters(blink::Plugin* plugin);
};

} // delay
