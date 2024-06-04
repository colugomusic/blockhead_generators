#pragma once

#include <blink/plugin_impl.hpp>

struct Params {
	struct {
		blink_ParamIdx attack;
		blink_ParamIdx release;
		blink_ParamIdx threshold;
		blink_ParamIdx ratio;
		blink_ParamIdx knee;
		blink_ParamIdx mix;
	} env;
	struct {
		blink_ParamIdx stereo;
	} option;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
