#pragma once

#include <blink/dsp.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>
#include "audio_data.h"
#include "model.h"

namespace dsp {

[[nodiscard]] static
auto get_analysis_data(const Model& model, blink_ID sample_id) -> const SampleAnalysis* {
	const auto pos = model.sample_analysis.find(sample_id);
	if (pos == model.sample_analysis.end()) {
		return nullptr;
	}
	return pos->second.get();
}

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_ParamData* param_data) -> AudioData {
	// TODO:
	return {};
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out) -> blink_Error {
	unit_dsp->block_positions.add(buffer.unit.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, unit_state.unit.param_data);
	// TODO:
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	// TODO:
}

} // dsp