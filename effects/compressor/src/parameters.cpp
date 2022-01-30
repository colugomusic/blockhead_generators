#include "parameters.h"
#include "parameters/attack.hpp"
#include "parameters/knee.hpp"
#include "parameters/mix.hpp"
#include "parameters/processing_mode.hpp"
#include "parameters/ratio.hpp"
#include "parameters/release.hpp"
#include "parameters/threshold.hpp"

using namespace blink;

namespace compressor {

Parameters::Parameters(blink::Plugin* plugin)
{
	env.attack = plugin->add_parameter(params::attack::envelope_parameter());
	env.release = plugin->add_parameter(params::release::envelope_parameter());
	env.threshold = plugin->add_parameter(params::threshold::envelope_parameter());
	env.ratio = plugin->add_parameter(params::ratio::envelope_parameter());
	env.knee = plugin->add_parameter(params::knee::envelope_parameter());
	env.mix = plugin->add_parameter(params::mix::envelope_parameter());
	options.stereo = plugin->add_parameter(params::processing_mode::option());

	plugin->add_manipulator_target(params::attack::UUID, params::attack::envelope_manipulator_target());
	plugin->add_manipulator_target(params::knee::UUID, params::knee::envelope_manipulator_target());
	plugin->add_manipulator_target(params::mix::UUID, params::mix::envelope_manipulator_target());
	plugin->add_manipulator_target(params::ratio::UUID, params::ratio::envelope_manipulator_target());
	plugin->add_manipulator_target(params::release::UUID, params::release::envelope_manipulator_target());
	plugin->add_manipulator_target(params::threshold::UUID, params::threshold::envelope_manipulator_target());
}

} // compressor
