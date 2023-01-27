#pragma once

#include <blink/standard_parameters/reverse.hpp>
#include "../parameters.h"

namespace fudge{
namespace params {
namespace reverse {

auto inline option()
{
	return blink::std_params::reverse::option();
}

auto inline toggle()
{
	auto out{blink::std_params::reverse::toggle()};

	out.manipulation_delegate = blink_Index(Parameters::Index::Option_ReverseMode);

	return out;
}


} // reverse
} // params
} // fudge
