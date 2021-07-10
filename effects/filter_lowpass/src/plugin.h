#pragma once

#include <blink/effect_plugin.hpp>

namespace lowpass {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "3bb94b4c-ec38-4557-8666-8fe2e446b83c";
	static constexpr auto NAME = "Lowpass";

	enum class ParameterIndex
	{
		Env_Freq,
		Env_Res,
		Env_Mix,
	};

	Plugin();

	const blink::EnvelopeParameter& env_freq() const { return *env_freq_; }
	const blink::EnvelopeParameter& env_res() const { return *env_res_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	blink::EffectInstance* make_instance() override;

	std::shared_ptr<blink::EnvelopeParameter> env_freq_;
	std::shared_ptr<blink::EnvelopeParameter> env_res_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};

} // lowpass