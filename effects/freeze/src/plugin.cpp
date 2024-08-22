#define BLINK_EXPORT

#include "dsp.hpp"
#include "model.h"
#include <blink_std.h>

static Model model;

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_effect_get_info(blink_InstanceIdx instance_idx) -> blink_EffectInstanceInfo {
	return {-1, -1, -1, -1};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid     = {"cde37ae4-a943-4b2c-b792-3eeba8107a6b"};
	out.name     = {"Freeze"};
	out.category = {BLINK_STD_CATEGORY_DESTRUCTION};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.pitch    = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.env.formant  = blink::add::param::env(model.plugin, {BLINK_STD_UUID_FORMANT});
	model.params.env.mix      = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
	model.params.slider.pitch = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::add_flags(model.plugin, model.params.env.pitch, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.formant, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.mix, blink_ParamFlags_DefaultActive);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.pitch, model.params.env.pitch);
	return BLINK_OK;
}

auto blink_instance_destroy(blink_InstanceIdx instance_idx) -> blink_Error {
	return blink::destroy_instance(&model.entities, instance_idx);
}

auto blink_instance_make() -> blink_InstanceIdx {
	return blink::make_instance(&model.entities);
}

auto blink_instance_reset(blink_InstanceIdx instance_idx) -> blink_Error {
	auto& instance_dsp = model.entities.instance.get<InstanceDSP>(instance_idx.value);
	dsp::instance_reset(&model, &instance_dsp);
	return BLINK_OK;
}

auto blink_instance_stream_init(blink_InstanceIdx instance_idx, blink_SR SR) -> blink_Error {
	return BLINK_OK;
}

auto blink_effect_process(blink_UnitIdx unit_idx, const blink_VaryingData* varying, const blink_UniformData* uniform, const float* in, float* out) -> blink_Error {
	const auto instance_idx = model.entities.unit.get<blink_InstanceIdx>(unit_idx.value);
	auto& instance_dsp      = model.entities.instance.get<InstanceDSP>(instance_idx.value);
	auto& unit_dsp          = model.entities.unit.get<UnitDSP>(unit_idx.value);
	return dsp::process(&model, unit_idx, &instance_dsp, &unit_dsp, *varying, *uniform, in, out);
}

auto blink_terminate() -> blink_Error {
	return blink::terminate(&model.entities);
}

auto blink_unit_add(blink_InstanceIdx instance_idx) -> blink_UnitIdx {
	return blink::add_unit(&model.entities, instance_idx);
}

auto blink_unit_reset(blink_UnitIdx unit_idx) -> blink_Error {
	const auto instance_idx = model.entities.unit.get<blink_InstanceIdx>(unit_idx.value);
	auto& instance_dsp      = model.entities.instance.get<InstanceDSP>(instance_idx.value);
	auto& unit_dsp          = model.entities.unit.get<UnitDSP>(unit_idx.value);
	dsp::unit_reset(&model, &instance_dsp, &unit_dsp);
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	const auto instance_idx = model.entities.unit.get<blink_InstanceIdx>(unit_idx.value);
	auto& instance_dsp      = model.entities.instance.get<InstanceDSP>(instance_idx.value);
	auto& unit_dsp          = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	dsp::unit_reset(&model, &instance_dsp, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(plugin);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::plugin::get_filesystem(), path);
}
