#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env pitch, mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.env.mix   = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data  = make_audio_data(*model, uniform.param_data);
	const auto pitch = blink::search::vec(data.env.pitch, unit_dsp->block_positions) + 60.0f;
	const auto mix   = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	const auto omega = blink::math::convert::pitch_to_frequency(pitch) / float(unit_dsp->SR.value);
	const auto sine = unit_dsp->sine(omega);
	const auto am = (sine + 1.0f) / 2.0f;
	const auto out_vec = ml::DSPVectorArray<2>(in) * ml::repeatRows<2>(ml::lerp(ml::DSPVector(1.0f), am, mix));
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->sine.clear();
}

} // dsp
