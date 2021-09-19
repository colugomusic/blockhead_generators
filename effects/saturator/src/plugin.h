#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace saturator {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "589e1a61-9bf3-4862-b2b8-7be4e01c0386";
	static constexpr auto NAME = "Saturator";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // saturator
