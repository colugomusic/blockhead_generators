#pragma once

#include <blink/plugin.hpp>
#include "parameters.h"

namespace berk {

class Berk : public blink::Plugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Berk";

	Berk();

	const auto& params() const { return params_; }

private:

	Parameters params_;
};

} // berk