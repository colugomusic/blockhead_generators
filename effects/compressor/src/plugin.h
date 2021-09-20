#pragma once

#include <blink/effect_plugin.hpp>
#include "parameters.h"

namespace compressor {

class Plugin : public blink::EffectPlugin
{
public:

	static constexpr auto UUID = "3117af4c-905c-4bba-8e4a-35360e7ad910";
	static constexpr auto NAME = "Compressor";

	Plugin();

	const auto& params() const { return params_; }

private:

	blink::EffectInstance* make_instance() override;

	Parameters params_;
};

} // compressor
