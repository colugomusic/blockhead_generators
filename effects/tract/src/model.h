#pragma once

#include <blink/plugin_impl.hpp>
#include <snd/audio/filter/tract.hpp>
#include <snd/resampler.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPBuffer.h>
#pragma warning(pop)

struct Params {
	struct {
		struct {
			blink_ParamIdx position;
			blink_ParamIdx diameter;
		} throat;
		struct {
			blink_ParamIdx position;
			blink_ParamIdx diameter;
		} tongue;
		blink_ParamIdx quality;
		blink_ParamIdx mix;
	} env;
};

struct InstanceInfo {
	blink_EffectInstanceInfo info;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::unique_ptr<ml::DSPBuffer> input_buffer;
	std::array<snd::audio::filter::tract::dsp, 2> tract;
	snd::resampler<2> resampler;
	snd::resampler<2> input_resampler;
};

using Instance = blink::Instance<InstanceInfo>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
