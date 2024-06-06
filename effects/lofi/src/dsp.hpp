#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env sr;
		blink::uniform::Env br;
		blink::uniform::Env mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.sr = blink::make_env_data(model.plugin, param_data, model.params.env.sr);
	out.env.br = blink::make_env_data(model.plugin, param_data, model.params.env.br);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	const auto sr = blink::search::vec(data.env.sr, unit_dsp->block_positions);
	const auto br = blink::search::vec(data.env.br, unit_dsp->block_positions);
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	const auto sr_to_inc = [](float SR, ml::DSPVector x) {
		x = (0.99f * (x * x * x * x)) + 0.01f; 
		return (x * SR) / SR;
	}; 
	const auto inc = sr_to_inc(float(unit_dsp->SR.value), sr); 
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec; 
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		unit_dsp->phase += inc[i]; 
		if (unit_dsp->phase >= 0.5f) {
			unit_dsp->value[0] = in_vec.constRow(0)[i];
			unit_dsp->value[1] = in_vec.constRow(1)[i];
			unit_dsp->phase -= 1.0f;
		}
		out_vec.row(0)[i] = unit_dsp->value[0];
		out_vec.row(1)[i] = unit_dsp->value[1];
	} 
	const auto br_to_step = [](ml::DSPVector x) {
		x -= 1; 
		return (x * x * x * x) * 0.95f;
	}; 
	const auto step = br_to_step(br); 
	out_vec = blink::math::stepify(out_vec, ml::repeatRows<2>(step)); 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->phase    = 0.0f;
	unit_dsp->value[0] = 0.0f;
	unit_dsp->value[1] = 0.0f;
}

} // dsp
