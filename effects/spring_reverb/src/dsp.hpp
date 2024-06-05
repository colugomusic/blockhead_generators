#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
	} env;
	struct {
		blink::OptionData stereo;
	} option;
	struct {
	} slider;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_ParamData* param_data) -> AudioData {
	AudioData out;
	// TODO:
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(buffer.unit.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, unit_state.unit.param_data);
	// TODO:
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	// TODO:
}

} // dsp
