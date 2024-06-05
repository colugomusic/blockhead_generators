#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/filter/2-pole_allpass_array.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx frequency;
		blink_ParamIdx resonance;
		blink_ParamIdx spread;
		blink_ParamIdx mix;
	} env;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::unique_ptr<snd::audio::filter::Filter_2Pole_AllpassArray<2, 32>> filter;
	std::array<ml::LinearGlide, 2> frequency_glide;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
