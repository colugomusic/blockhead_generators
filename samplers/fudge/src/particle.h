#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

#include "grain.h"

namespace fudge {

class Controller;

struct Particle {
	int harmonic = 0;
	bool trig_primed = false;
	char flip_flop = 0;
	float trigger_timer = 0.0f;
	std::array<Grain, 2> grains;
};

auto init(Particle* p, int harmonic) -> void;
auto process(Particle* p, Controller* controller, const ml::DSPVector& amp) -> ml::DSPVectorArray<2>;

} // fudge
