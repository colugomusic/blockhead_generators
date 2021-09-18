#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace lofi {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "90e5db1b-19c0-4ad5-aecf-3622db865584";
	static constexpr auto NAME = "Lofi";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // lofi
