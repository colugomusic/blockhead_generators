#pragma once

#include "sample_analysis.h"
#include <blink.h>
#include <blink/plugin_impl.hpp>
#include <map>
#include <memory>

struct Params {
	struct {
	} env;
	struct {
	} option;
	struct {
	} slider_int;
	struct {
	} slider_real;
};

struct DrawState {
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
	DrawState draw;
	Params params;
	std::map<blink_ID, std::shared_ptr<SampleAnalysis>> sample_analysis;
};
