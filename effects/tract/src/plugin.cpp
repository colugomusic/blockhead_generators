#define BLINK_EXPORT

#include "dsp.hpp"
#include "model.h"
#include "shared/tract_params.hpp"
#include <blink_std.h>

static Model model;

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_effect_get_info(blink_InstanceIdx instance_idx) -> blink_EffectInstanceInfo {
	return model.entities.instance.get<InstanceInfo>(instance_idx.value).info;
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid     = {"7bd10f49-7262-4d9b-9004-bcf47d971fc3"};
	out.name     = {"Tract"};
	out.category = {BLINK_STD_CATEGORY_FILTERS"|Special"};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.throat.position = tract::add::param::env::throat_position(model.plugin);
	model.params.env.throat.diameter = tract::add::param::env::throat_diameter(model.plugin);
	model.params.env.tongue.position = tract::add::param::env::tongue_position(model.plugin);
	model.params.env.tongue.diameter = tract::add::param::env::tongue_diameter(model.plugin);
	model.params.env.quality         = tract::add::param::env::quality(model.plugin);
	model.params.env.mix             = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
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
	blink_EffectInstanceInfo info = {-1, -1, -1, -1};
	const auto MIN_MODEL_SR = dsp::BASE_MODEL_SR / 2;
	const auto MAX_MODEL_SR = dsp::BASE_MODEL_SR * 2;
	info.min_delay = int((float(SR.value) / MAX_MODEL_SR) * kFloatsPerDSPVector);
	info.max_delay = int((float(SR.value) / MIN_MODEL_SR) * kFloatsPerDSPVector);
	model.entities.instance.get<InstanceInfo>(instance_idx.value) = InstanceInfo{info};
	return BLINK_OK;
}

auto blink_effect_process(blink_UnitIdx unit_idx, const blink_VaryingData* varying, const blink_UniformData* uniform, const float* in, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	return dsp::process(&model, &unit_dsp, *varying, *uniform, in, out);
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
	dsp::init(&model, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(plugin);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::plugin::get_filesystem(), path);
}