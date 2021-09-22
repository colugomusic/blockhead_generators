#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace wavebender {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "08304d60-c45d-4712-b492-688b4d5b8cf2";
	static constexpr auto NAME = "Wavebender";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // wavebender