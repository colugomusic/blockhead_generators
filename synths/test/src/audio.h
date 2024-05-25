/* TODELETE:
#pragma once

#define _USE_MATH_DEFINES

#include <blink/synth_unit.hpp>
#include "shared/noise_generator.h"

namespace test {

class Plugin;
class Instance;

class Audio : public blink::SynthUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
};

} // test
*/