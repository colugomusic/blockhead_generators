#pragma once

#include <blink/plugin_impl.hpp>

struct Params {
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx pan;
	} env;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx pan;
	} slider;
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
