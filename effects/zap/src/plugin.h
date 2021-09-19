#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace zap {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "74cfcda1-1810-4852-a9b4-7a0c074d3424";
	static constexpr auto NAME = "Zap";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // zap
