#pragma once

#include <blink/effect_plugin.hpp>

namespace spring_reverb {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "0af95a47-f5f5-426c-8384-00f6ed30449d";
	static constexpr auto NAME = "Spring Reverb";

	enum class ParameterIndex
	{
		Env_Size = 0,
		Env_Decay = 1,
		Env_Mix = 2,
	};

	Plugin();

	const blink::EnvelopeParameter& env_size() const { return *env_size_; }
	const blink::EnvelopeParameter& env_decay() const { return *env_decay_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	blink::EffectInstance* make_instance() override;

	std::shared_ptr<blink::EnvelopeParameter> env_size_;
	std::shared_ptr<blink::EnvelopeParameter> env_decay_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};

} // spring_reverb