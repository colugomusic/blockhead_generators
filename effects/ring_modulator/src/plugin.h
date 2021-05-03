#pragma once

#include <blink/plugin.hpp>

class RingModulator : public blink::Plugin
{
public:

	static constexpr auto UUID = "881569c5-108b-4923-b30e-1e1ac6c1a04f";
	static constexpr auto NAME = "Ring Modulator";

	enum class ParameterIndex
	{
		Env_Pitch,
		Env_Mix,
	};

	RingModulator();

	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};