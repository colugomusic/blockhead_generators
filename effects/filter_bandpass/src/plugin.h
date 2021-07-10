#pragma once

#include <blink/effect_plugin.hpp>

namespace bandpass {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "e3d0105f-8d22-4dba-b041-d21a52d0f930";
	static constexpr auto NAME = "Bandpass";

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

} // bandpass