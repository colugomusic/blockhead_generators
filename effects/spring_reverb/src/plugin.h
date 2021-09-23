#pragma once

#include <blink/effect_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace spring_reverb {

class Plugin : public blink::EffectPlugin
{
public:

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override { return new Instance(this); }

	Parameters params_;
};

} // spring_reverb