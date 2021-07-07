#pragma once

#include <blink/plugin.hpp>
#include "parameters.h"

class Berk : public blink::Plugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Berk";

	enum class ParameterIndex
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

	Berk();

	const auto& params() const { return params_; }

private:

	Parameters params_;
};