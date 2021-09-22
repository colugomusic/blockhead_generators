#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/option_spec.hpp>
#include <blink/plugin.hpp>

namespace wavebender {

struct Parameters
{
	enum class Index
	{
		Env_Bubble,
		Env_Tilt,
		Env_Pitch,
		Env_XFadeSize,
		Env_Mix,

		Opt_XFadeMode,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> bubble;
		std::shared_ptr<blink::EnvelopeParameter> tilt;
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> xfade_size;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	struct Options
	{
		std::shared_ptr<blink::OptionParameter> xfade_mode;
	} opt;

	Parameters(blink::Plugin* plugin);
};

} // wavebender