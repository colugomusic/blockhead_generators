#pragma once

#include <blink/synth_plugin.hpp>
#include "parameters.h"

namespace berk {

class Plugin : public blink::SynthPlugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Berk";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::SynthInstance* make_instance() override;

	Parameters params_;
};

} // berk