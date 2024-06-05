#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/saturator/moron_saturator.hpp>

struct Params {
	struct {
		blink_ParamIdx drive;
		blink_ParamIdx amp;
		blink_ParamIdx mix;
	} env;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	snd::audio::saturator::MoronSaturator<2> saturator;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
