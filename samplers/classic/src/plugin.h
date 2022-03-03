#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "instance.h"
#include "parameters.h"

namespace classic {

class Plugin : public blink::SamplerPlugin
{
public:

	const Parameters params;

	Plugin() : params(this) {}

	GUI& gui() { return gui_; }

private:

	auto make_instance() -> std::unique_ptr<blink::SamplerInstance> override { return std::make_unique<Instance>(this); }

	GUI gui_;
};

} // classic