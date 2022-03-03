#pragma once

#include <blink/effect_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace lowpass {

class Plugin : public blink::EffectPlugin
{
public:

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

private:

	auto make_instance() -> std::unique_ptr<blink::EffectInstance> override { return std::make_unique<Instance>(this); }

	Parameters params_;
};

} // lowpass