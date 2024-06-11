#define BLINK_EXPORT

#include "dsp.hpp"
#include "model.h"
#include <blink_std.h>

static Model model;

[[nodiscard]]
auto add_param_env_drive(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"ad7727c7-f73c-49ff-919d-9a48a75995a4"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Drive"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_effect_get_info(blink_InstanceIdx instance_idx) -> blink_EffectInstanceInfo {
	return {-1, -1, -1, -1};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid     = {"589e1a61-9bf3-4862-b2b8-7be4e01c0386"};
	out.name     = {"Saturator"};
	out.category = {BLINK_STD_CATEGORY_DESTRUCTION};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.drive = add_param_env_drive(model.plugin);
	model.params.env.amp = blink::add::param::env(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.env.mix = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
	blink::write::param::add_flags(model.plugin, model.params.env.amp, blink_ParamFlags_DefaultActive);
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
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	return BLINK_OK;
}

CMRC_DECLARE(plugin);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::plugin::get_filesystem(), path);
}