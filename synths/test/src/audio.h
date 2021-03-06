#pragma once

#define _USE_MATH_DEFINES

#include <blink/synth_unit.hpp>
#include <bleat/oscillators/scalar/multi_wave.hpp>
#include "shared/noise_generator.h"

namespace test {

class Plugin;
class Instance;

class Audio : public blink::SynthUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
	std::array<bleat::oscillators::scalar::MultiWaveOsc, 2> oscs_;
	NoiseGenerator noise_gen_;
};

} // test