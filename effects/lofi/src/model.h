#pragma once

#include <blink/plugin_impl.hpp>

struct Params {
	struct {
		blink_ParamIdx sr;
		blink_ParamIdx br;
		blink_ParamIdx mix;
	} env;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	float phase = 0.0f;
	std::array<float, 2> value;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
