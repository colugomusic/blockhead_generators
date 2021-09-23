#pragma once

#include <map>
#include <memory>
#include <blink/effect_plugin.hpp>
#include "instance.h"

namespace freeze {

class Plugin : public blink::EffectPlugin
{
public:

	enum class ParameterIndex
	{
		Env_Pitch,
		Env_Formant,
		Env_Mix,
		Sld_Pitch,
	};

	Plugin();

	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_formant() const { return *env_formant_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	blink::EffectInstance* make_instance() override { return new Instance(this); }

	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_formant_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};

} // freeze