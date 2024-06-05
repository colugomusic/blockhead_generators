#pragma once

#include <blink/plugin_impl.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPBuffer.h>
#include <DSP/MLDSPFilters.h>
#pragma warning(pop)

struct Params {
	struct {
		blink_ParamIdx tilt;
		blink_ParamIdx xfade_size;
		blink_ParamIdx pitch;
		blink_ParamIdx bubble;
		blink_ParamIdx smoother;
		blink_ParamIdx mix;
	} env;
	struct {
		blink_ParamIdx xfade_mode;
	} option;
};

enum class CrossfadeMode {
	Static,
	Dynamic,
};

struct FrameWriteParams {
	int bubble = 0;
};

struct FrameReadParams {
	float tilt = 0.0f;
	float spike = 0.0f;
	float ff = 1.0f;
	float crossfade_size = 1.0f;
	CrossfadeMode crossfade_mode = CrossfadeMode::Dynamic;
};

struct Channel {
	std::array<std::vector<float>, 4> buffers;
	struct Span {
		float* buffer = nullptr;
		size_t size = 0;
	};
	struct {
		Span span;
		bool up = false;
		int counter = 0;
		ml::Lopass filter;
	} write; 
	struct {
		Span span;
	} stage; 
	struct {
		Span span;
		float frame = 0.0f;
	} source; 
	struct {
		Span span;
		float frame = 0.0f;
	} target; 
	struct {
		float source_speed_0 = 1.0f;
		float source_speed_1 = 1.0f;
		float target_speed_0 = 1.0f;
		float target_speed_1 = 1.0f;
		bool active = false;
		size_t length = 64;
		size_t index = 0;
	} xfade;
	struct {
		bool active = false;
		size_t length = 64;
		size_t index = 0;
	} fade_in;
	int init = 0;
};

struct UnitDSP {
	blink_SR SR;
	blink::BlockPositions block_positions;
	std::array<Channel, 2> channels;
};

using Instance = blink::Instance<>;
using Unit     = blink::Unit<UnitDSP>;

struct Model {
	blink::Plugin plugin;
	blink::Entities<Instance, Unit> entities;
	Params params;
};
