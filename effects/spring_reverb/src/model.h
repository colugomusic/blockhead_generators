#pragma once

#include <blink/plugin_impl.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx size;
		blink_ParamIdx decay;
		blink_ParamIdx mix;
	} env;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	ml::LinearGlide glide_size;
	ml::LinearGlide glide_decay;
	ml::Allpass<ml::PitchbendableDelay> ap1, ap2, ap3, ap4;
	ml::Allpass<ml::PitchbendableDelay> ap5, ap6, ap7, ap8, ap9, ap10;
	ml::PitchbendableDelay delayL, delayR;
	ml::DSPVector feedbackL, feedbackR;
	int buffer_count = 0;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
