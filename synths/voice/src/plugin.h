#pragma once

#include <blink/plugin.hpp>

class Voice : public blink::Plugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Voice";

	enum class ParameterIndex
	{
		Env_Amp,
	};

	Voice();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
};