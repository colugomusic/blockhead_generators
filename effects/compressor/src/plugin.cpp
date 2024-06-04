#define BLINK_EXPORT

#include <blink_std.h>
#include "dsp.hpp"
#include "model.h"

static Model model;

[[nodiscard]]
auto add_param_env_attack(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"568d2cba-2fbb-4536-83d1-44781f8b0cab"});
	const auto env_idx   = blink::add::env::ms<1, 10000, 1000, 1>(plugin.host, 0.1f);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Attack"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_effect_get_info(blink_InstanceIdx instance_idx) -> blink_EffectInstanceInfo {
	return {0};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid     = {"3117af4c-905c-4bba-8e4a-35360e7ad910"};
	out.name     = {"Compressor"};
	out.category = {BLINK_STD_CATEGORY_DYNAMICS};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.attack = add_param_env_attack(model.plugin);
	//model.params.env.release = add_param_env_release(model.plugin);
	//model.params.env.threshold = add_param_env_threshold(model.plugin);
	//model.params.env.ratio = add_param_env_ratio(model.plugin);
	//model.params.env.knee = add_param_env_knee(model.plugin);
	model.params.env.mix = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
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

auto blink_effect_process(blink_UnitIdx unit_idx, const blink_EffectBuffer* buffer, const blink_EffectUnitState* unit_state, const float* in, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	// TODO: return dsp::process(&model, &unit_dsp, *buffer, *unit_state, in, out);
	return {};
}

auto blink_terminate() -> blink_Error {
	return blink::terminate(&model.entities);
}

auto blink_unit_add(blink_InstanceIdx instance_idx) -> blink_UnitIdx {
	return blink::add_unit(&model.entities, instance_idx);
}

auto blink_unit_reset(blink_UnitIdx unit_idx) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	// TODO: dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	// TODO: dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(compressor);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::compressor::get_filesystem(), path);
}