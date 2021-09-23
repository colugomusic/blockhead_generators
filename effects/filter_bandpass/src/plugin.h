#pragma once

#include <blink/effect_plugin.hpp>
#include "instance.h"

namespace bandpass {

class Plugin : public blink::EffectPlugin
{
public:

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

	blink::EffectInstance* make_instance() override { return new Instance(this); }

	std::shared_ptr<blink::EnvelopeParameter> env_freq_;
	std::shared_ptr<blink::EnvelopeParameter> env_res_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};

} // bandpass