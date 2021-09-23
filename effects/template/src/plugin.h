#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace template {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "742b3f81-c51b-42ab-8b01-5ddf717b322a";
	static constexpr auto NAME = "Template";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // template
