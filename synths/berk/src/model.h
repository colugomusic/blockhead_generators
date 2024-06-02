#pragma once

#include <blink/plugin_impl.hpp>

struct Params {
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx buzz;
		blink_ParamIdx fricative_intensity;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
		blink_ParamIdx quality;
		struct {
			blink_ParamIdx diameter;
			blink_ParamIdx position;
		} throat;
		struct {
			blink_ParamIdx diameter;
			blink_ParamIdx position;
		} tongue;
	} env;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
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
