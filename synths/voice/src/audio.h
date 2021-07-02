#pragma once

#define _USE_MATH_DEFINES

#include <random>
#include <blink.h>
#include <blink/synth.hpp>
#include "glottis.h"
#include "tract.h"

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Voice;

class Audio : public blink::Synth
{
public:

	Audio(Voice* plugin, int instance_group);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	void reset() override;

private:

	ml::Bandpass aspirate_filter_;
	ml::Bandpass fricative_filter_;
	ml::NoiseGen noise_;
	const Voice* plugin_;

	Glottis glottis_;
	Tract tract_;
};