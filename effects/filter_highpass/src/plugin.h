#pragma once

#include <blink/effect_plugin.hpp>

namespace highpass {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "bcf8e814-e0df-41a4-b16a-0344aa391c44";
	static constexpr auto NAME = "Highpass";

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

} // highpass