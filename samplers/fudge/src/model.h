#pragma once

#include "sample_analysis.h"
#include "shared/noise_generator.h"
#include <blink.h>
#include <blink/plugin_impl.hpp>
#include <blink/transform/stretch.hpp>
#include <map>
#include <memory>
#include <snd/audio/fudge.hpp>
#include <snd/audio/scale.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx harmonics_scale;
	} chord;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
		blink_ParamIdx speed;
		struct {
			blink_ParamIdx size;
			blink_ParamIdx transpose;
			blink_ParamIdx uniformity;
		} grain;
		struct {
			blink_ParamIdx amount;
			blink_ParamIdx spread;
		} harmonics;
		struct {
			blink_ParamIdx amount;
			blink_ParamIdx color;
		} noise;
	} env;
	struct {
		blink_ParamIdx noise_mode;
		blink_ParamIdx reverse_mode;
	} option;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx noise_width;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
		blink_ParamIdx sample_offset;
		blink_ParamIdx speed;
	} slider;
	struct {
		blink_ParamIdx loop;
		blink_ParamIdx reverse;
	} toggle;
};

struct DrawState {
	blink::transform::Stretch stretch_transformer;
};

struct Controller {
	blink::transform::Stretch stretch_transformer;
	ml::DSPVector ff;
	ml::DSPVector spread;
	ml::DSPVectorInt scale = ml::DSPVectorInt();
	snd::fudge::vector_info v;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	blink::Traverser block_traverser;
	NoiseGenerator noise_gen;
	Controller controller;
	std::array<snd::fudge::particle, 4> particles;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	DrawState draw;
	Params params;
	std::map<blink_ID, std::shared_ptr<SampleAnalysis>> sample_analysis;
};
