#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env drive, amp, mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.drive = blink::make_env_data(model.plugin, param_data, model.params.env.drive);
	out.env.amp = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	const auto drive = blink::search::vec(data.env.drive, unit_dsp->block_positions);
	const auto amp = blink::search::vec(data.env.amp, unit_dsp->block_positions);
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	ml::DSPVectorArray<2> in_vec(in);
	auto out_vec = unit_dsp->saturator(in_vec, ml::repeatRows<2>(drive)) * ml::repeatRows<2>(amp);
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

} // dsp
