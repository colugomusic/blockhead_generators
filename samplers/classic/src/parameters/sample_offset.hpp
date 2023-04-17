#pragma once

#include <blink/standard_parameters/sample_offset.hpp>

namespace classic {
namespace params {
namespace sample_offset {

static constexpr auto UUID { blink::std_params::sample_offset::UUID };

auto inline slider_parameter() {
	return blink::std_params::sample_offset::slider_parameter();
}

} // sample_offset
} // params
} // classic
