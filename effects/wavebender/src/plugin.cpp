#define BLINK_EXPORT

#include "dsp.hpp"
#include "model.h"
#include <blink_std.h>

static Model model;

[[nodiscard]]
auto add_param_env_tilt(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"196b2999-7979-4b2e-bbe2-2b8684d4af80"});
	const auto env_idx   = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Tilt"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::clamp_range(plugin, param_idx, {-1.0f, 1.0f});
	return param_idx;
}

[[nodiscard]]
auto add_param_env_xfade_size(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"d9f04877-408d-4411-8983-21f66fc149a8"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Crossfade"});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::env::default_value(plugin, env_idx, 0.25f);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_bubble(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"2306f951-daca-4833-a175-258dc5407b02"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Bubble"});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	return param_idx;
}

[[nodiscard]]
auto add_param_env_smoother(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"357b372b-f5d1-4e41-be6c-24dda95d6ad4"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Smoother"});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	return param_idx;
}

[[nodiscard]]
auto add_param_option_xfade_mode(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::option(plugin, {"32e0b599-2dc9-4dfc-b7fa-e284e72289b5"});
	const auto flags     = blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Crossfade Mode"});
	blink::write::param::strings(plugin, param_idx, {{"Static", "Dynamic"}});
	blink::write::param::option_default_value(plugin, param_idx, 1);
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
	out.uuid     = {"08304d60-c45d-4712-b492-688b4d5b8cf2"};
	out.name     = {"Wavebender"};
	out.category = {BLINK_STD_CATEGORY_DESTRUCTION};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.tilt          = add_param_env_tilt(model.plugin);
	model.params.env.xfade_size    = add_param_env_xfade_size(model.plugin);
	model.params.env.pitch         = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.env.bubble        = add_param_env_bubble(model.plugin);
	model.params.env.smoother      = add_param_env_smoother(model.plugin);
	model.params.env.mix           = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
	model.params.option.xfade_mode = add_param_option_xfade_mode(model.plugin);
	blink::write::param::add_flags(model.plugin, model.params.env.pitch, blink_ParamFlags_DefaultActive);
	blink::write::param::add_subparam(model.plugin, model.params.env.xfade_size, model.params.option.xfade_mode);
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