#pragma once

#include <blink/plugin.hpp>

class Freeze : public blink::Plugin
{
public:

	static constexpr auto UUID = "cde37ae4-a943-4b2c-b792-3eeba8107a6b";
	static constexpr auto NAME = "Freeze";

	enum class ParameterIndex
	{
		Env_Pitch,
		Env_Formant,
		Env_Mix,
		Sld_Pitch,
	};

	Freeze();

	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_formant() const { return *env_formant_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_formant_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};