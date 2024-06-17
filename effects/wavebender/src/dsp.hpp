#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

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

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	for (int c = 0; c < 2; c++) {
		snd::wavebender::reset(&unit_dsp->channels[c]);
	}
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data  = make_audio_data(*model, uniform.param_data);
	auto bubble      = blink::search::vec(data.env.bubble, unit_dsp->block_positions);
	auto tilt        = blink::search::vec(data.env.tilt, unit_dsp->block_positions) * 8.0f;
	auto pitch       = blink::math::convert::p_to_ff(blink::search::vec(data.env.pitch, unit_dsp->block_positions));
	auto xfade_size  = blink::search::vec(data.env.xfade_size, unit_dsp->block_positions);
	auto smoother    = blink::math::convert::linear_to_filter_hz(1.0f - blink::search::one(data.env.smoother, unit_dsp->block_positions)) / unit_dsp->SR.value;
	const auto mix   = blink::search::vec(data.env.mix, unit_dsp->block_positions); 
	xfade_size       = ml::lerp(ml::DSPVector(2.0f), ml::DSPVector(32.0f), xfade_size * xfade_size); 
	auto bubble_int  = ml::roundFloatToInt(bubble * bubble * 64.0f);
	const auto spike = ml::clamp(ml::abs(tilt * 0.25f) - 1.0f, ml::DSPVector(0.0f), ml::DSPVector(1.0f)); 
	tilt = ml::clamp(tilt, ml::DSPVector(-4.0f), ml::DSPVector(4.0f)); 
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;
	ml::DSPVectorArray<2> smoothed_input; 
	for (int c = 0; c < 2; c++) {
		smoothed_input.row(c) = snd::wavebender::apply_smoother(&unit_dsp->channels[c], in_vec.constRow(c), smoother);
	} 
	const auto& block_pos = unit_dsp->block_positions; 
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		if (block_pos[i - 1] < 0 && block_pos[i] >= 0) {
			reset(model, unit_dsp);
		} 
		for (int c = 0; c < 2; c++) {
			out_vec.row(c)[i] = snd::wavebender::generate_frame(
				&unit_dsp->channels[c],
				in_vec.constRow(c)[i],
				smoothed_input.constRow(c)[i],
				snd::wavebender::CrossfadeMode(data.option.xfade_mode.value),
				bubble_int[i],
				spike[i],
				tilt[i],
				pitch[i],
				xfade_size[i]);
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
		snd::wavebender::init(&unit_dsp->channels[c], unit_dsp->SR.value);
	}
	reset(model, unit_dsp);
}

} // dsp
