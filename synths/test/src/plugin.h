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

	blink::SynthInstance* make_instance() override { return new Instance(this); }

	Parameters params_;
};

} // test