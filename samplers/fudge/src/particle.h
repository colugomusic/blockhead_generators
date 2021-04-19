#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

#include "grain.h"

class Controller;

class Particle
{
public:

	Particle(const Controller& controller, int harmonic = 0);

	ml::DSPVectorArray<2> process();

private:

	void reset(int index);
	void trigger_next_grain(int index, bool adjust);
	float adjust_channel_pos(int index, int channel, float pos) const;
	float get_mono_position(int index, float pos, bool adjust) const;
	std::array<float, 2> get_stereo_positions(int index, float pos, bool adjust) const;
	float read_mono_frame(const Grain& grain) const;
	std::array<float, 2> read_stereo_frame(const Grain& grain) const;

	Grain& other_grain(int idx) { return grains_[idx == 0 ? 1 : 0]; }
	const Grain& other_grain(int idx) const { return grains_[idx == 0 ? 1 : 0]; }

	const Controller* controller_;
	int harmonic_ = 0;
	bool trig_primed_ = false;
	char flip_flop_ = 0;
	float trigger_timer_ = 0.0f;
	std::array<Grain, 2> grains_;

};