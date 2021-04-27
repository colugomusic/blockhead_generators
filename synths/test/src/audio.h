#pragma once

#define _USE_MATH_DEFINES

#include <blink.h>
#include <blink/synth.hpp>
#include <bleat/oscillators/scalar/multi_wave.hpp>

#include "shared/noise_generator.h"

class Test;

class Audio : public blink::Synth
{
public:

	Audio(const Test* plugin);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	blink_Error reset() override;

private:
	
	const Test* plugin_;
	std::array<bleat::oscillators::scalar::MultiWaveOsc, 2> oscs_;
	NoiseGenerator noise_gen_;
};