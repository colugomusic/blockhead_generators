#pragma once

#include <blink/plugin_impl.hpp>
#include "shared/tract.h"
#include "shared/resampler.h"
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

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::unique_ptr<ml::DSPBuffer> input_buffer;
	std::array<::Tract, 2> tract;
	Resampler<2> resampler;
	Resampler<2> input_resampler;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};