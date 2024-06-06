#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env freq;
		blink::uniform::Env res;
		blink::uniform::Env mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.freq = blink::make_env_data(model.plugin, param_data, model.params.env.frequency);
	out.env.res = blink::make_env_data(model.plugin, param_data, model.params.env.resonance);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	const auto freq = blink::search::one(data.env.freq, unit_dsp->block_positions);
	auto res = blink::search::one(data.env.res, unit_dsp->block_positions); 
	res = ml::lerp(1.0f, 0.1f, res); 
	const auto omega = blink::math::convert::linear_to_filter_hz(freq) / unit_dsp->SR.value;
	const auto coeffs = ml::Bandpass::coeffs(omega, res); 
	unit_dsp->filter[0].mCoeffs = coeffs;
	unit_dsp->filter[1].mCoeffs = coeffs; 
	const ml::DSPVectorArray<2> in_vec(in); 
	const auto L = unit_dsp->filter[0](in_vec.constRow(0));
	const auto R = unit_dsp->filter[1](in_vec.constRow(1)); 
	auto out_vec = ml::concatRows(L, R); 
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions); 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->filter[0] = {};
	unit_dsp->filter[1] = {};
}

} // dsp
