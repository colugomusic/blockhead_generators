#pragma once

#include <blink/effect_plugin.hpp>
#include "instance.h"
#include "parameters.h"

namespace template {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "742b3f81-c51b-42ab-8b01-5ddf717b322a";
	static constexpr auto NAME = "Template";

	Plugin() : params_(this) {}

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override { return new Instance(this); }

	Parameters params_;
};

} // template
