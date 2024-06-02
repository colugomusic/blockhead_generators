#pragma once

#include "model.h"
#include <blink/plugin_impl.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>

struct DrawData {
	struct {
	} env;
	struct {
	} slider;
	struct {
	} option;
	struct {
	} toggle;
};

[[nodiscard]]
auto make_draw_data(const Model& model, const blink_ParamData* param_data) -> DrawData {
	// TODO:
	return {};
}

[[nodiscard]]
auto draw(Model* model, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	const auto data        = make_draw_data(*model, unit_state.unit.param_data);
	const auto sample_data = blink::SampleData{buffer.sample_info, unit_state.channel_mode};
	// TODO:
	return BLINK_OK;
}
