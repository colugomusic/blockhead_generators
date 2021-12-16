#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "instance.h"
#include "parameters.h"

namespace classic {

class Plugin : public blink::SamplerPlugin
{
public:

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

	GUI& gui() { return gui_; }

private:

	blink::SamplerInstance* make_instance() override { return new Instance(this); }

	GUI gui_;

	Parameters params_;
};

} // classic