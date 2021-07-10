#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace tract {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "7bd10f49-7262-4d9b-9004-bcf47d971fc3";
	static constexpr auto NAME = "Tract";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // tract