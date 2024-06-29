#define BLINK_EXPORT

#include <blink_std.h>
#include "dsp.hpp"
#include "model.h"
#include "shared/tract_params.hpp"

static Model model;

using namespace blink;

auto add_param_env_amp(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {BLINK_STD_UUID_AMP});
	const auto flags = blink_ParamFlags_DefaultActive;
	const auto env_idx = blink::read::env(model.plugin, param_idx);
	blink::write::env::default_value(model.plugin, env_idx, 0.5f);
	blink::write::param::add_flags(model.plugin, param_idx, blink_ParamFlags_DefaultActive);
	return param_idx;
}

auto add_param_env_pitch(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {BLINK_STD_UUID_PITCH});
	const auto flags = blink_ParamFlags_DefaultActive;
	const auto env_idx = blink::read::env(model.plugin, param_idx);
	blink::write::param::add_flags(model.plugin, param_idx, blink_ParamFlags_DefaultActive);
	blink::write::param::group(model.plugin, param_idx, {"Voice"});
	return param_idx;
}

auto add_param_env_buzz(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"a62d1f52-d493-4902-b92d-c09f5a92e8d2"});
	const auto env_idx = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(model.plugin, param_idx, {"Buzz"});
	blink::write::param::group(model.plugin, param_idx, {"Voice"});
	blink::write::param::clamp_range(model.plugin, param_idx, {-1.0f, 1.0f});
	blink::write::param::add_flags(model.plugin, param_idx, flags);
	blink::write::param::env(model.plugin, param_idx, env_idx);
	blink::write::param::offset_env(model.plugin, param_idx, env_idx);
	blink::write::param::override_env(model.plugin, param_idx, env_idx);
	return param_idx;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = blink_PluginInfo();
	out.uuid     = {"e7804e4e-fdf6-4545-aa4f-77fed08d892a"};
	out.name     = {"Berk"};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.amp = add_param_env_amp(model.plugin);
	model.params.env.pan = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.env.pitch = add_param_env_pitch(model.plugin);
	model.params.env.buzz = add_param_env_buzz(model.plugin);
	model.params.env.throat.position = tract::add::param::env::throat_position(model.plugin);
	model.params.env.throat.diameter = tract::add::param::env::throat_diameter(model.plugin);
	model.params.env.tongue.position = tract::add::param::env::tongue_position(model.plugin);
	model.params.env.tongue.diameter = tract::add::param::env::tongue_diameter(model.plugin);
	model.params.env.fricative_intensity = tract::add::param::env::fricative_intensity(model.plugin);
	model.params.env.quality = tract::add::param::env::quality(model.plugin);
	model.params.slider.amp = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.slider.pan = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.slider.pitch = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::manip_delegate(model.plugin, model.params.slider.amp, model.params.env.amp);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.pan, model.params.env.pan);
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
	return BLINK_OK;
}

auto blink_instance_stream_init(blink_InstanceIdx instance_idx, blink_SR SR) -> blink_Error {
	return BLINK_OK;
}

auto blink_synth_process(blink_UnitIdx unit_idx, const blink_VaryingData* varying, const blink_UniformData* uniform, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	return dsp::process(&model, &unit_dsp, *varying, *uniform, out);
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