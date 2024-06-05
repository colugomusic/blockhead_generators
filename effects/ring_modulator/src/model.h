#pragma once

#include <blink/plugin_impl.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx pitch;
		blink_ParamIdx mix;
	} env;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	ml::SineGen sine;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
