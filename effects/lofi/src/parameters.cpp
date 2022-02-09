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

	plugin->add_manipulator_target(params::sample_rate::UUID, params::sample_rate::envelope_manipulator_target());
	plugin->add_manipulator_target(params::bit_reduction::UUID, params::bit_reduction::envelope_manipulator_target());
	plugin->add_manipulator_target(std_params::mix::UUID, std_params::mix::envelope_manipulator_target());
}

} // lofi
