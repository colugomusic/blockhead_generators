#pragma once

#include "model.h"
#include <blink/search.hpp>
#include <snd/misc.hpp>

namespace dsp {

static constexpr auto INIT = 4;

struct AudioData {
	struct {
		blink::uniform::Env tilt;
		blink::uniform::Env xfade_size;
		blink::uniform::Env pitch;
		blink::uniform::Env bubble;
		blink::uniform::Env smoother;
		blink::uniform::Env mix;
	} env;
	struct {
		blink::uniform::Option xfade_mode;
	} option;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.bubble = blink::make_env_data(model.plugin, param_data, model.params.env.bubble);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	out.env.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.env.smoother = blink::make_env_data(model.plugin, param_data, model.params.env.smoother);
	out.env.tilt = blink::make_env_data(model.plugin, param_data, model.params.env.tilt);
	out.env.xfade_size = blink::make_env_data(model.plugin, param_data, model.params.env.xfade_size);
	out.option.xfade_mode = blink::make_option_data(model.plugin, param_data, model.params.option.xfade_mode);
	return out;
}

auto do_write(Channel* c, const FrameWriteParams& params, float filtered_value, float value) -> void {
	if (c->init == 0) {
		if (std::abs(value) < 0.000001f) return; 
		c->init++;
	}
	c->write.span.buffer[c->write.span.size++] = value; 
	if (c->write.span.size >= c->buffers[0].size()) {
		c->write.span.size = c->buffers[0].size();
		std::swap(c->write.span, c->stage.span);
		c->write.up = false;
		return;
	}
	if (c->write.up && filtered_value < 0) {
		c->write.up = false;
	}
	else if (!c->write.up && filtered_value > 0) {
		c->write.up = true; 
		if (c->write.span.size > 3) {
			c->write.counter++; 
			if (c->write.counter > params.bubble) {
				if (++c->init >= INIT) {
					std::swap(c->write.span, c->stage.span);
				} 
				c->write.span.size = 0;
				c->write.counter = 0;
			}
		}
	}
}

auto start_fade_in(Channel* c, const FrameReadParams& params) -> void {
	const auto tmp = c->target.span;
	c->target.span = c->stage.span;
	c->target.frame = 0.0f;
	c->stage.span = c->source.span;
	c->stage.span.size = 0;
	c->source.span = tmp;
	c->source.frame = c->target.frame; 
	c->fade_in.active = true;
	c->fade_in.index = 0;
	c->fade_in.length = c->target.span.size * 2;
}

[[nodiscard]]
auto window(float x, float r = 0.5f) -> float {
	const auto top = 1.0f - r;
	if (x < r) {
		return blink::math::ease::parametric::in_out(x * (1.0f / r));
	}
	else if (x > top) {
		return 1.0f - blink::math::ease::parametric::in_out((x - top) * (1.0f / r));
	}
	else {
		return 1.0f;
	}
}

[[nodiscard]]
auto apply_tilt(float frame, float tilt, float spike, size_t size) -> float {
	const auto pos = frame / size;
	const auto tilted = std::pow(pos, std::pow(2.0f, tilt));
	const auto smoothed = blink::math::lerp(pos, tilted, window(pos));
	return blink::math::lerp(smoothed, tilted, spike) * size;
}

[[nodiscard]]
auto read(const Channel::Span& span, float pos) -> float {
	auto index_0 = size_t(std::floor(pos));
	auto index_1 = size_t(std::ceil(pos));
	const auto x = pos - index_0;
	index_0 = snd::wrap(index_0, span.size);
	index_1 = snd::wrap(index_1, span.size);
	if (index_0 == index_1) {
		return span.buffer[index_0];
	} 
	const auto value_0 = span.buffer[index_0];
	const auto value_1 = span.buffer[index_1];
	return blink::math::lerp(value_0, value_1, x);
}

[[nodiscard]]
auto do_xfade(Channel* c, const FrameReadParams& params) -> float {
	const auto x = blink::math::ease::quadratic::in_out(float(c->xfade.index) / (c->xfade.length - 1));
	auto source_value = read(c->source.span, apply_tilt(c->source.frame, params.tilt, params.spike, c->source.span.size));
	auto target_value = read(c->target.span, apply_tilt(c->target.frame, params.tilt, params.spike, c->target.span.size));
	const auto value = blink::math::lerp(source_value, target_value, x);
	const auto source_inc = blink::math::lerp(c->xfade.source_speed_0, c->xfade.source_speed_1, x) * params.ff;
	const auto target_inc = blink::math::lerp(c->xfade.target_speed_0, c->xfade.target_speed_1, x) * params.ff; 
	c->source.frame += source_inc;
	c->target.frame += target_inc; 
	const auto source_end = (c->source.span.size - 1);
	const auto target_end = (c->target.span.size - 1); 
	if (c->source.frame > source_end) c->source.frame -= source_end;
	if (c->target.frame > target_end) c->target.frame -= target_end; 
	c->xfade.index++; 
	if (c->xfade.index >= c->xfade.length) {
		c->xfade.active = false;
	} 
	return value;
}

auto prepare_xfade(Channel* c, const FrameReadParams& params) -> void {
	const auto tmp = c->target.span;
	c->target.span = c->stage.span;
	c->target.frame = 0.0f;
	c->stage.span = c->source.span;
	c->stage.span.size = 0;
	c->source.span = tmp;
	c->source.frame = c->target.frame;
}

auto start_xfade(Channel* c, const FrameReadParams& params) -> void {
	c->xfade.active = true;
	c->xfade.index = 0;
	if (params.crossfade_mode == CrossfadeMode::Dynamic) {
		c->xfade.length = size_t(float(blink::math::midpoint(c->source.span.size, c->target.span.size)) * params.crossfade_size);
	}
	else {
		c->xfade.length = size_t(64.0f * params.crossfade_size);
	} 
	c->xfade.source_speed_1 = float(c->source.span.size) / float(c->target.span.size);
	c->xfade.target_speed_0 = float(c->target.span.size) / float(c->source.span.size);
}

[[nodiscard]]
auto do_wet(Channel* c, const FrameReadParams& params) -> float {
	if (c->xfade.active) {
		return do_xfade(c, params);
	}
	auto value = read(c->target.span, apply_tilt(c->target.frame, params.tilt, params.spike, c->target.span.size));
	c->target.frame += params.ff; 
	const auto end = (c->target.span.size - 1); 
	if (c->target.frame > end) {
		c->target.frame -= end; 
		if (c->stage.span.size > 0) {
			prepare_xfade(c, params);
			start_xfade(c, params);
		}
	} 
	return value;
}

[[nodiscard]]
auto do_read(Channel* c, const FrameReadParams& params, float in) -> float {
	if (c->init == INIT) {
		start_fade_in(c, params);
		c->init++;
	}
	if (c->init < INIT) {
		return in;
	}
	float value = do_wet(c, params);
	if (c->fade_in.active) {
		const auto amp = blink::math::ease::quadratic::in_out(float(c->fade_in.index++) / c->fade_in.length);
		value = blink::math::lerp(in, value, amp);
		if (c->fade_in.index >= c->fade_in.length) {
			c->fade_in.active = false;
		}
	} 
	return value;
}

[[nodiscard]]
auto process(Channel* c, const FrameWriteParams& write_params, const FrameReadParams& read_params, float in, float filtered_in) -> float {
	do_write(c, write_params, filtered_in, in);
	return do_read(c, read_params, in);
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	for (int c = 0; c < 2; c++) {
		unit_dsp->channels[c].init = 0;
		unit_dsp->channels[c].fade_in.active = false;
		unit_dsp->channels[c].xfade.active = false;
		unit_dsp->channels[c].write.span.buffer = unit_dsp->channels[c].buffers[0].data();
		unit_dsp->channels[c].write.span.size = 0;
		unit_dsp->channels[c].write.counter = 0;
		unit_dsp->channels[c].write.up = false;
		unit_dsp->channels[c].stage.span.buffer = unit_dsp->channels[c].buffers[1].data();
		unit_dsp->channels[c].stage.span.size = 0;
		unit_dsp->channels[c].source.span.buffer = unit_dsp->channels[c].buffers[2].data();
		unit_dsp->channels[c].source.span.size = 0;
		unit_dsp->channels[c].target.span.buffer = unit_dsp->channels[c].buffers[3].data();
		unit_dsp->channels[c].target.span.size = 0;
	}
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	auto bubble = blink::search::vec(data.env.bubble, unit_dsp->block_positions);
	auto tilt = blink::search::vec(data.env.tilt, unit_dsp->block_positions) * 8.0f;
	auto pitch = blink::math::convert::p_to_ff(blink::search::vec(data.env.pitch, unit_dsp->block_positions));
	auto crossfade_size = blink::search::vec(data.env.xfade_size, unit_dsp->block_positions);
	auto smoother = blink::math::convert::linear_to_filter_hz(1.0f - blink::search::one(data.env.smoother, unit_dsp->block_positions)) / unit_dsp->SR.value;
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions); 
	crossfade_size = ml::lerp(ml::DSPVector(1.0f), ml::DSPVector(32.0f), crossfade_size * crossfade_size); 
	auto bubble_int = ml::roundFloatToInt(bubble * bubble * 64.0f);
	const auto spike = ml::clamp(ml::abs(tilt * 0.25f) - 1.0f, ml::DSPVector(0.0f), ml::DSPVector(1.0f)); 
	tilt = ml::clamp(tilt, ml::DSPVector(-4.0f), ml::DSPVector(4.0f)); 
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;
	ml::DSPVectorArray<2> filtered_input; 
	for (int c = 0; c < 2; c++) {
		unit_dsp->channels[c].write.filter.mCoeffs = ml::Lopass::coeffs(smoother, 1.0f);
		filtered_input.row(c) = unit_dsp->channels[c].write.filter(in_vec.constRow(c));
	} 
	const auto& block_pos = unit_dsp->block_positions; 
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		if (block_pos[i - 1] < 0 && block_pos[i] >= 0) {
			reset(model, unit_dsp);
		} 
		for (int c = 0; c < 2; c++) {
			FrameWriteParams write_params; 
			write_params.bubble = bubble_int[i]; 
			FrameReadParams read_params; 
			read_params.crossfade_size = crossfade_size[i];
			read_params.crossfade_mode = CrossfadeMode(data.option.xfade_mode.value);
			read_params.tilt = tilt[i];
			read_params.spike = spike[i];
			read_params.ff = pitch[i]; 
			out_vec.row(c)[i] = process(&unit_dsp->channels[c], write_params, read_params, in_vec.constRow(c)[i], filtered_input.constRow(c)[i]);
		}
	} 
	ml::validate(out_vec.constRow(0)); 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	for (int c = 0; c < 2; c++) {
		for (int i = 0; i < 4; i++) {
			unit_dsp->channels[c].buffers[i].resize(unit_dsp->SR.value);
		}
	}
	reset(model, unit_dsp);
}

} // dsp
