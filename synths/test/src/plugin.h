#pragma once

#include <blink/synth_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace test {

class Plugin : public blink::SynthPlugin
{
public:

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

private:

	auto make_instance() -> std::unique_ptr<blink::SynthInstance> override { return std::make_unique<Instance>(this); }

	Parameters params_;
};

} // test