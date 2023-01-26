#pragma once

#include <blink/parameters/option_spec.hpp>
#include <blink/standard_parameters/search.hpp>

namespace compressor {
namespace params {
namespace processing_mode {

static constexpr auto UUID { "47601288-d3d0-4d35-9cd3-f2e985b49cde" };

inline auto option()
{
	blink::OptionSpec out;

	out.uuid = UUID;
	out.name = "Processing Mode";

	out.default_index = 0;
	out.options.push_back("Mono");
	out.options.push_back("Stereo");

	return out;
}

} // processing_mode
} // params
} // compressor
