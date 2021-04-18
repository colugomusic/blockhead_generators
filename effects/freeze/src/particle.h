#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

#include "grain.h"

class Controller;

struct LR { float L = 0.0f, R = 0.0f; };

class Particle
{
public:

	Particle(const Controller& controller, std::function<float(int, std::size_t, float)> read);

	LR process(int vector_);
	void clear();

private:

	void reset(int index);
	void trigger_next_grain(int index);
	LR read_stereo_frame(int vector_index, const Grain& grain) const;

	Grain& other_grain(int idx) { return grains_[1 - idx]; }
	const Grain& other_grain(int idx) const { return grains_[1 - idx]; }

	const Controller* controller_;
	bool first_grain_ = true;
	char flip_flop_ = 0;
	float trigger_timer_ = 0.0f;
	std::array<Grain, 2> grains_;
	std::function<float(int, std::size_t, float)> read_;

};