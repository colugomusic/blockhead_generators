#pragma once

#include <blink/effect_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace saturator {

class Plugin : public blink::EffectPlugin
{
public:

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

private:

	auto make_instance() -> std::unique_ptr<blink::EffectInstance> override { return std::make_unique<Instance>(this); }

	Parameters params_;
};

} // saturator
