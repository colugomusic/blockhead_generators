#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "instance.h"
#include "manipulator_targets.h"
#include "parameters.h"

namespace classic {

class Plugin : public blink::SamplerPlugin
{
public:

	Plugin() : manipulator_targets_(this), params_(this) {}

	const auto& manipulator_targets() const { return manipulator_targets_; }
	const auto& params() const { return params_; }

	GUI& gui() { return gui_; }

private:

	blink::SamplerInstance* make_instance() override { return new Instance(this); }

	GUI gui_;

	ManipulatorTargets manipulator_targets_;
	Parameters params_;
};

} // classic