#pragma once

#include <bleat/oscillators/scalar/multi_wave.hpp>
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
	blink::BlockPositions block_positions;
	std::array<bleat::oscillators::scalar::MultiWaveOsc, 2> oscs;
	ml::LinearGlide fade_in;
	NoiseGenerator noise_gen;
};

using Instance = blink::ent::Instance<>;
using Unit     = blink::ent::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
