#include "parameters.h"
#include <blink/standard_parameters/mix.hpp>
#include "parameters/bit_reduction.hpp"
#include "parameters/sample_rate.hpp"

using namespace blink;

namespace lofi {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.sr = plugin->add_parameter(params::sample_rate::envelope_parameter());
	env.br = plugin->add_parameter(params::bit_reduction::envelope_parameter());
	env.mix = plugin->add_parameter(std_params::mix::envelope_parameter());
}

} // lofi
