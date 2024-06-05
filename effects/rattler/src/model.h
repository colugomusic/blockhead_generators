#pragma once

#include <blink/plugin_impl.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx time;
		blink_ParamIdx feedback;
		blink_ParamIdx width;
		blink_ParamIdx dry;
		blink_ParamIdx wet;
	} env;
	struct {
		blink_ParamIdx dry;
		blink_ParamIdx wet;
		blink_ParamIdx width;
	} slider;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::array<ml::PitchbendableDelay, 2> delays;
	ml::DSPVectorArray<2> feedback;
	ml::LinearGlide fade_in;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
