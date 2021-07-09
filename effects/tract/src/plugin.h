#pragma once

#include <blink/plugin.hpp>
#include "parameters.h"

namespace tract {

class Tract : public blink::Plugin
{
public:

	static constexpr auto UUID = "7bd10f49-7262-4d9b-9004-bcf47d971fc3";
	static constexpr auto NAME = "Tract";

	Tract();

	const auto& params() const { return params_; }

private:

	Parameters params_;
};

} // tract