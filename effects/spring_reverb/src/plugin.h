#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace spring_reverb {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "0af95a47-f5f5-426c-8384-00f6ed30449d";
	static constexpr auto NAME = "Spring Reverb";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // spring_reverb