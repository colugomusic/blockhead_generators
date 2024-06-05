#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {
  
static constexpr auto BASE_MODEL_SR = 44100;
static constexpr auto ROWS = 2;

struct AudioData {
	struct {
		struct {
			blink::EnvData position;
			blink::EnvData diameter;
		} throat;
		struct {
			blink::EnvData position;
			blink::EnvData diameter;
		} tongue;
		blink::EnvData quality;
		blink::EnvData mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_ParamData* param_data) -> AudioData {
	AudioData out;
	out.env.throat.position = blink::make_env_data(model.plugin, param_data, model.params.env.throat.position);
	out.env.throat.diameter = blink::make_env_data(model.plugin, param_data, model.params.env.throat.diameter);
	out.env.tongue.position = blink::make_env_data(model.plugin, param_data, model.params.env.tongue.position);
	out.env.tongue.diameter = blink::make_env_data(model.plugin, param_data, model.params.env.tongue.diameter);
	out.env.quality         = blink::make_env_data(model.plugin, param_data, model.params.env.quality);
	out.env.mix             = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

template <size_t ROWS> static
auto is_silent(const ml::DSPVectorArray<ROWS>& x) -> bool {
	static constexpr auto THRESHOLD = 0.00001f;
	for (int r{0}; r < ROWS; r++) {
		if (ml::max(ml::abs(x.constRow(r))) > THRESHOLD) return false;
	}
	return true;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(buffer.unit.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, unit_state.unit.param_data);
	const auto index = blink::search::vec(data.env.throat.position, unit_dsp->block_positions);
	const auto diameter = blink::search::vec(data.env.throat.diameter, unit_dsp->block_positions);
	const auto tongue_index = blink::search::vec(data.env.tongue.position, unit_dsp->block_positions);
	const auto tongue_diameter = blink::search::vec(data.env.tongue.diameter, unit_dsp->block_positions);
	const auto quality = blink::search::one(data.env.quality, unit_dsp->block_positions);
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	static const ml::DSPVector MIN_INDEX(4.0f);
	static const ml::DSPVector MAX_INDEX(44.0f);
	static const ml::DSPVector MIN_DIAMETER(-1.0f);
	static const ml::DSPVector MAX_DIAMETER(3.0f);
	static const ml::DSPVector MIN_TONGUE_INDEX(12.0f);
	static const ml::DSPVector MAX_TONGUE_INDEX(29.0f);
	static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
	static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);
	const auto min_position = ml::min(ml::intToFloat(unit_dsp->block_positions.positions.pos));
	const auto gate = min_position >= 0.0f; 
	ml::DSPVectorArray<2> in_vec(in); 
	unit_dsp->input_buffer->write(in_vec); 
	const auto model_SR = int(std::pow(2.0f, quality - 1.0f) * float(BASE_MODEL_SR));
	const auto speed = float(model_SR) / unit_dsp->SR.value;
	const auto required_frames = (1.0f / speed) * kFloatsPerDSPVector; 
	ml::DSPVectorArray<2> out_vec; 
	if (unit_dsp->input_buffer->getReadAvailable() >= required_frames * 2) {
		auto source = [&]() {
			auto input_source = [&]() {
				ml::DSPVectorArray<2> out; 
				unit_dsp->input_buffer->read(out); 
				return out;
			}; 
			ml::DSPVectorArray<2> tract_in{unit_dsp->input_resampler(input_source, 1.0f / speed)};
			ml::DSPVectorArray<2> tract_out; 
			if (is_silent(tract_in)) return tract_out; 
			for (int r = 0; r < 2; r++) {
				::Tract::Input tract_input; 
				tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, diameter);
				tract_input.fricative_intensity = 0.0f;
				tract_input.glottal_output = tract_in.constRow(r);
				tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, blink::math::convert::bi_to_uni(index));
				tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, tongue_diameter);
				tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, blink::math::convert::bi_to_uni(tongue_index)); 
				tract_out.row(r) = unit_dsp->tract[r](model_SR, speed, tract_input);
			} 
			return tract_out;
		}; 
		out_vec = unit_dsp->resampler(source, speed);
	} 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	const auto MIN_MODEL_SR = BASE_MODEL_SR / 2;
	const auto buffer_size = 2 * int(std::ceil(kFloatsPerDSPVector * ROWS * (float(unit_dsp->SR.value) / float(MIN_MODEL_SR))));
	unit_dsp->input_buffer = std::make_unique<ml::DSPBuffer>();
	unit_dsp->input_buffer->resize(buffer_size);
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	for (int r = 0; r < 2; r++) {
		unit_dsp->tract[r].reset();
	}
}

} // dsp