#pragma once

#include <blink/envelope_spec.hpp>
#include <blink/parameters/option_spec.hpp>
#include <blink/plugin.hpp>

namespace tract {

struct Parameters
{
	enum class Index
	{
		Env_Index,
		Env_Diameter,
		Env_TongueIndex,
		Env_TongueDiameter,
		Env_Quality,
		Env_Mix,
	};

	struct Envelopes
	{
		std::shared_ptr<blink::EnvelopeParameter> index;
		std::shared_ptr<blink::EnvelopeParameter> diameter;
		std::shared_ptr<blink::EnvelopeParameter> tongue_index;
		std::shared_ptr<blink::EnvelopeParameter> tongue_diameter;
		std::shared_ptr<blink::EnvelopeParameter> quality;
		std::shared_ptr<blink::EnvelopeParameter> mix;
	} env;

	Parameters(blink::Plugin* plugin);
};

}