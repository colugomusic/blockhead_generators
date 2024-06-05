#pragma once

#include "buffer.h"
#include <blink/plugin_impl.hpp>

struct Grain {
	bool on = false;
	bool dry = false;
	float size = 1024.0f;
	float period = 1024.0f;
	float window = 256.0f;
	float frame = 0.0f;
	float frame_amp = 0.0f;
	float duck = 1.0f;
	float ff = 1.0f;
};

struct Particle {
	bool first_grain = true;
	char flip_flop = 0;
	float trigger_timer = 0.0f;
	std::array<Grain, 2> grains;
	bool queue_reset = false;
};

struct Params {
	struct {
		blink_ParamIdx pitch;
		blink_ParamIdx formant;
		blink_ParamIdx mix;
	} env;
	struct {
		blink_ParamIdx pitch;
	} slider;
};

struct InstanceDSP {
	std::optional<blink_UnitIdx> master_unit;
	freeze::FreezeBuffer buffer;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	Particle particle;
	bool record = false;
};

using Instance = blink::Instance<InstanceDSP>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
