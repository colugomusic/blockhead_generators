/* TODELETE:
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

struct playback_positions {
	std::array<double, kFloatsPerDSPVector> value;
};

struct sample_positions {
	std::array<double, kFloatsPerDSPVector> value;
};

auto init(Particle* p, int harmonic) -> void;
auto process(Particle* p, const Controller& controller, const playback_positions& positions, const ml::DSPVector& amp) -> ml::DSPVectorArray<2>;

} // fudge
*/
