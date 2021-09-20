#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace compressor {

struct Parameters
{
	enum class Index
	{
		Env_Attack,
		Env_Release,
		Env_Threshold,
		Env_Ratio,
		Env_Knee,
		Env_Mix,

		Opt_Stereo,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> attack;
		std::shared_ptr<blink::EnvelopeParameter> release;
		std::shared_ptr<blink::EnvelopeParameter> threshold;
		std::shared_ptr<blink::EnvelopeParameter> ratio;
		std::shared_ptr<blink::EnvelopeParameter> knee;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Options
	{
		std::shared_ptr<blink::OptionParameter> stereo;
	} options;

	Parameters(blink::Plugin* plugin);
};

} // compressor
