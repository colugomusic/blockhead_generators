#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/env_follower.hpp>

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
	struct {
		snd::audio::EnvFollowerAR<1> env_follower_0;
		snd::audio::EnvFollower<1> env_follower_1;
	} mono;
	struct {
		snd::audio::EnvFollowerAR<2> env_follower_0;
		snd::audio::EnvFollower<2> env_follower_1;
	} stereo;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
