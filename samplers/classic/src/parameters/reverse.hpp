#pragma once

#include <blink/standard_parameters/reverse.hpp>
#include <blink/parameters/option_spec.hpp>

namespace classic {
namespace params {
namespace reverse {

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
} // classic
