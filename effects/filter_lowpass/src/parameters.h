#pragma once

#include <blink/parameters/chord_spec.hpp>
#include <blink/envelope_spec.hpp>
#include <blink/parameters/slider_parameter_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace lowpass {

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

} // lowpass
