#define BLINK_EXPORT

#include <blink_std.h>
#include "draw.hpp"
#include "dsp.hpp"
#include "model.h"

static Model model;

using namespace blink;

[[nodiscard]] static
auto make_sampler_info() -> blink_SamplerInfo {
	blink_SamplerInfo out = {0};
	out.baked_waveform_could_be_different = {false};
	out.requires_preprocessing            = {true};
	return out;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid                = {"795c8dc6-3b81-4397-abac-071bab83b10f"};
	out.name                = {"Fudge"};
	out.version             = {PLUGIN_VERSION};
	out.enable_warp_markers = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host, make_sampler_info());
	// TODO:
	return BLINK_OK;
}

auto blink_instance_destroy(blink_InstanceIdx instance_idx) -> blink_Error {
	return blink::destroy_instance(&model.entities, instance_idx);
}

auto blink_instance_make() -> blink_InstanceIdx {
	return blink::make_instance(&model.entities);
}

auto blink_instance_reset(blink_InstanceIdx instance_idx) -> blink_Error {
	return BLINK_OK;
}

auto blink_instance_stream_init(blink_InstanceIdx instance_idx, blink_SR SR) -> blink_Error {
	return BLINK_OK;
}

auto blink_sampler_preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info) -> blink_Error {
	auto pos = model.sample_analysis.find(sample_info->id);
	if (pos != model.sample_analysis.end()) {
		return BLINK_OK;
	}
	auto analysis = std::make_shared<SampleAnalysis>();
	if (analyze(host, callbacks, *sample_info, analysis.get())) {
		model.sample_analysis[sample_info->id] = analysis;
	}
	return BLINK_OK;
}

auto blink_sampler_sample_deleted(blink_ID sample_id) -> blink_Error {
	const auto pos = model.sample_analysis.find(sample_id);
	if (pos == model.sample_analysis.end()) return BLINK_OK;
	model.sample_analysis.erase(pos);
	return BLINK_OK;
}

auto blink_sampler_draw(const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	return draw(&model, *buffer, *unit_state, n, out);
}

auto blink_sampler_process(blink_UnitIdx unit_idx, const blink_SamplerBuffer* buffer, const blink_SamplerUnitState* unit_state, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	return dsp::process(&model, &unit_dsp, *buffer, *unit_state, out);
}

auto blink_terminate() -> blink_Error {
	return blink::terminate(&model.entities);
}

auto blink_unit_add(blink_InstanceIdx instance_idx) -> blink_UnitIdx {
	return blink::add_unit(&model.entities, instance_idx);
}

auto blink_unit_reset(blink_UnitIdx unit_idx) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	return BLINK_OK;
}
