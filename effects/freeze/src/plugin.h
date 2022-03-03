#pragma once

#include <map>
#include <memory>
#include <blink/effect_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace freeze {

class Plugin : public blink::EffectPlugin
{
public:

	Plugin() : parameters_(this) {}

	const auto& params() const { return parameters_; }

private:

	auto make_instance() -> std::unique_ptr<blink::EffectInstance> override { return std::make_unique<Instance>(this); }

	Parameters parameters_;
};

} // freeze