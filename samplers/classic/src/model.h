#pragma once

#include <blink/blink_plugin_impl.hpp>
#include <blink/transform/tape.hpp>
#include "shared/noise_generator.h"

struct Params {
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx noise_amount;
		blink_ParamIdx noise_color;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
	} env;
	struct {
		blink_ParamIdx loop;
		blink_ParamIdx noise_mode;
		blink_ParamIdx reverse_mode;
		blink_ParamIdx reverse_toggle;
	} option;
	struct {
		blink_ParamIdx samp_offs;
	} slider_int;
	struct {
		blink_ParamIdx amp;
		blink_ParamIdx noise_width;
		blink_ParamIdx pan;
		blink_ParamIdx pitch;
	} slider_real;
};

struct ReverseCorrection {
	struct Grain {
		bool on {};
		float beg {};
		float pos {};
		float ff {};
		float vpos {};
		float vend {};
		float vff {};
	} grain;
	blink::BlockPositions dry_positions;
};

struct UnitDSP {
	blink::BlockPositions block_positions;
	blink::transform::Tape tape_transformer;
	NoiseGenerator noise_gen;
	ReverseCorrection reverse_correction;
};

namespace ent {

using Instance = blink::ent::Instance<>;
using Unit     = blink::ent::Unit<UnitDSP>;

} // ent

struct Model {
	blink::Plugin plugin;
	blink::Entities<::ent::Instance, ::ent::Unit> entities;
	Params params;
};
