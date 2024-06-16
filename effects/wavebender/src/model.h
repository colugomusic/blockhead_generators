#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/wavebender.hpp>

struct Params {
	struct {
		blink_ParamIdx tilt;
		blink_ParamIdx xfade_size;
		blink_ParamIdx pitch;
		blink_ParamIdx bubble;
		blink_ParamIdx smoother;
		blink_ParamIdx mix;
	} env;
	struct {
		blink_ParamIdx xfade_mode;
	} option;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::array<snd::wavebender::Channel, 2> channels;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
