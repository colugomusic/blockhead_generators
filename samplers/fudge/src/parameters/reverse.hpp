#pragma once

#include <blink/standard_parameters/reverse.hpp>

namespace fudge{
namespace params {
namespace reverse {

static constexpr auto UUID { blink::std_params::reverse::UUID };

auto inline option()
{
	return blink::std_params::reverse::option();
}

auto inline toggle()
{
	return blink::std_params::reverse::toggle();
}


} // reverse
} // params
} // fudge
