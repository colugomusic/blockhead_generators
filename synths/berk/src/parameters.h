#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace berk {

struct Parameters
{
	enum class Index
	{
		Env_Amp,
		Env_Pan,
		Env_Pitch,
		Env_Buzz,
		Env_Index,
		Env_Diameter,
		Env_TongueIndex,
		Env_TongueDiameter,
		Env_FricativeIntensity,
		Env_Quality,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> amp;
		std::shared_ptr<blink::EnvelopeParameter> pan;
		std::shared_ptr<blink::EnvelopeParameter> pitch;
		std::shared_ptr<blink::EnvelopeParameter> buzz;
		std::shared_ptr<blink::EnvelopeParameter> index;
		std::shared_ptr<blink::EnvelopeParameter> diameter;
		std::shared_ptr<blink::EnvelopeParameter> tongue_index;
		std::shared_ptr<blink::EnvelopeParameter> tongue_diameter;
		std::shared_ptr<blink::EnvelopeParameter> fricative_intensity;
		std::shared_ptr<blink::EnvelopeParameter> quality;
	} env;

	Parameters(blink::Plugin* plugin);
};

}