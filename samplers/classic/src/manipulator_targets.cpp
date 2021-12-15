#include "manipulator_targets.h"
#include <blink/standard_parameters/all.hpp>

using namespace blink;

namespace classic {

ManipulatorTargets::ManipulatorTargets(Plugin* plugin)
{
	sliders.amp = plugin->add_manipulator_target(std_params::amp::manipulator_target());
	sliders.pan = plugin->add_manipulator_target(std_params::pan::manipulator_target());
	sliders.pitch = plugin->add_manipulator_target(std_params::pitch::manipulator_target());
}

} // classic
