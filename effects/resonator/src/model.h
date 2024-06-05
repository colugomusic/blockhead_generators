#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/feedback_delay.hpp>
#include <snd/audio/filter/1-pole.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		struct {
			blink_ParamIdx scale;
		} harmonics;
	} chord;
	struct {
		blink_ParamIdx pitch;
		blink_ParamIdx feedback;
		blink_ParamIdx damper;
		blink_ParamIdx mix;
		struct {
			blink_ParamIdx amount;
			blink_ParamIdx ratio;
		} fm;
		struct {
			blink_ParamIdx amount;
			blink_ParamIdx spread;
			blink_ParamIdx scale_snap_amount;
			blink_ParamIdx width;
		} harmonics;
	} env;
	struct {
		blink_ParamIdx pitch;
	} slider;
};

struct Resonator {
	snd::audio::FeedbackDelay<2> delay;
	snd::audio::filter::Filter_1Pole<2> filter; 
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	float SR_f;
	ml::DSPVector SR_vec;
	Resonator fundamental;
	std::array<Resonator, 3> harmonics;
	ml::SineGen fm_source;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
