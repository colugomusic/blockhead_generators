#define BLINK_EXPORT

#include "dsp.hpp"
#include "model.h"
#include <blink_std.h>

static Model model;

static constexpr auto UUID_WIDTH = "aec5e808-dcbb-424c-95a6-ba4749edccc4";

[[nodiscard]]
auto add_param_env_width(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {UUID_WIDTH});
	const auto env_idx   = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Width"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

[[nodiscard]]
auto add_param_slider_width(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::slider_real(plugin, {UUID_WIDTH});
	const auto sld_idx   = blink::add::slider::percentage_bipolar(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Width"});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::slider(plugin, param_idx, sld_idx);
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
	out.uuid     = {"4371f6cb-d452-49c2-af70-79663402c816"};
	out.name     = {"Rattler"};
	out.category = {BLINK_STD_CATEGORY_DELAY};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.time     = blink::add::param::env(model.plugin, {BLINK_STD_UUID_DELAY_TIME});
	model.params.env.feedback = blink::add::param::env(model.plugin, {BLINK_STD_UUID_FEEDBACK});
	model.params.env.width    = add_param_env_width(model.plugin);
	model.params.env.dry      = blink::add::param::env(model.plugin, {BLINK_STD_UUID_DRY});
	model.params.env.wet      = blink::add::param::env(model.plugin, {BLINK_STD_UUID_WET});
	model.params.slider.dry   = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_DRY});
	model.params.slider.wet   = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_WET});
	model.params.slider.width = add_param_slider_width(model.plugin);
	blink::write::param::add_flags(model.plugin, model.params.env.time, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.feedback, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.dry, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.wet, blink_ParamFlags_DefaultActive);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.dry, model.params.env.dry);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.wet, model.params.env.wet);
	blink::write::slider::default_value(model.plugin, blink::read::slider_real(model.plugin, model.params.slider.dry), 1.0f);
	blink::write::slider::default_value(model.plugin, blink::read::slider_real(model.plugin, model.params.slider.wet), 1.0f);
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