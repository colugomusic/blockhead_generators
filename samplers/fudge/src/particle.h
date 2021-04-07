#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

#include "grain.h"

class Controller;

class Particle
{
public:

	Particle(const Controller& controller);

	ml::DSPVectorArray<2> process();

private:

	void reset(int index);
	void trigger_next_grain(int index, bool adjust);
	float adjust_channel_pos(int index, int channel, float pos);

	Grain& other_grain(int idx) { return grains_[idx == 0 ? 1 : 0]; }

	const Controller* controller_;
	bool trig_primed_ = false;
	char flip_flop_ = 0;
	float trigger_timer_ = 0.0f;
	std::array<Grain, 2> grains_;

};