#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <random>
#include <blink/synth_unit.hpp>
#include "glottis.h"
#include "shared/tract.h"
#include "shared/resampler.h"

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace berk {

class Plugin;
class Instance;

class Audio : public blink::SynthUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	void reset() override;

private:

	ml::Bandpass aspirate_filter_;
	ml::Bandpass fricative_filter_;
	ml::NoiseGen noise_;
	const Plugin* plugin_;

	Glottis glottis_;
	Tract tract_;
	Resampler<1> resampler_;
};

} // berk