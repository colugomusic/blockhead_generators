#pragma once

#include <snd/audio/oscillators.hpp>
#include <blink/plugin_impl.hpp>
#include "shared/noise_generator.h"

struct Params {
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx pan;
		struct {
			blink_ParamIdx fm;
			blink_ParamIdx pitch;
			blink_ParamIdx waveform;
		} carrier;
		struct {
			blink_ParamIdx fm;
			blink_ParamIdx pitch;
		} modulator;
		struct {
			blink_ParamIdx amount;
			blink_ParamIdx color;
		} noise;
	} env;
	struct {
		blink_ParamIdx noise_mode;
	} option;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx carrier_pitch;
		blink_ParamIdx noise_width;
		blink_ParamIdx pan;
	} slider_real;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::array<snd::osc::scalar::MultiWaveOsc, 2> oscs;
	ml::LinearGlide fade_in;
	NoiseGenerator noise_gen;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
