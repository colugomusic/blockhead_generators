#define BLINK_EXPORT

#include <blink_std.h>
#include "dsp.hpp"
#include "model.h"

static Model model;

using namespace blink;

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid     = {"bb0c455a-dae3-498d-b929-f18a20764528"};
	out.name     = {"test synth"};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto add_param_env_amp(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {BLINK_STD_UUID_AMP});
	const auto flags = blink_ParamFlags_DefaultActive;
	const auto env_idx = blink::read::env(model.plugin, param_idx);
	blink::write::env::default_value(model.plugin, env_idx, 0.5f);
	blink::write::param::add_flags(model.plugin, param_idx, blink_ParamFlags_DefaultActive);
	return param_idx;
}

auto add_param_env_carrier_fm(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {"05ffbc42-d2aa-4746-905e-44d4373b4345"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::group(plugin, idx, {"Carrier"});
	blink::write::param::name(plugin, idx, {"Carrier FM Amount"});
	blink::write::param::short_name(plugin, idx, {"FM Amount"});
	blink::write::param::add_flags(plugin, idx, flags);
	blink::write::param::env(plugin, idx, blink::add::env::percentage(plugin.host));
	blink::write::param::clamp_range(plugin, idx, {0.0f, 2.0f});
	blink::write::param::offset_env(plugin, idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, idx, blink::add::env::percentage(plugin.host));
	return idx;
}

auto add_param_env_carrier_pitch(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_PITCH});
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::group(plugin, idx, {"Carrier"});
	blink::write::param::name(plugin, idx, {"Carrier Pitch"});
	blink::write::param::short_name(plugin, idx, {"Pitch"});
	blink::write::param::add_flags(plugin, idx, flags);
	return idx;
}

auto add_param_env_carrier_waveform(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {"337b8f6f-1d7a-41a7-ab05-802b23887b7a"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	const auto env_idx = blink::add::env::percentage(plugin.host);
	blink::write::param::group(plugin, idx, {"Carrier"});
	blink::write::param::name(plugin, idx, {"Waveform"});
	blink::write::param::add_flags(plugin, idx, flags);
	blink::write::param::clamp_range(plugin, idx, {0.0f, 1.0f});
	blink::write::param::env(plugin, idx, env_idx);
	blink::write::param::offset_env(plugin, idx, env_idx);
	blink::write::param::override_env(plugin, idx, blink::add::env::percentage(plugin.host));
	return idx;
}

auto add_param_env_modulator_fm(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {"343689ff-d1bf-48f0-96bf-7a6689568ef5"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::group(plugin, idx, {"Modulator"});
	blink::write::param::name(plugin, idx, {"Modulator FM Amount"});
	blink::write::param::short_name(plugin, idx, {"FM Amount"});
	blink::write::param::add_flags(plugin, idx, flags);
	blink::write::param::env(plugin, idx, blink::add::env::percentage(plugin.host));
	blink::write::param::clamp_range(plugin, idx, {0.0f, 2.0f});
	blink::write::param::offset_env(plugin, idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, idx, blink::add::env::percentage(plugin.host));
	return idx;
}

auto add_param_env_modulator_pitch(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_PITCH});
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::uuid(plugin, idx, {"fc48a5e2-48a6-4021-8467-c196b215938d"});
	blink::write::param::group(plugin, idx, {"Modulator"});
	blink::write::param::name(plugin, idx, {"Modulator Pitch"});
	blink::write::param::short_name(plugin, idx, {"Pitch"});
	blink::write::param::add_flags(plugin, idx, flags);
	return idx;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.amp                   = add_param_env_amp(model.plugin);
	model.params.env.pan                   = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.env.carrier.pitch         = add_param_env_carrier_pitch(model.plugin);
	model.params.env.carrier.fm            = add_param_env_carrier_fm(model.plugin);
	model.params.env.carrier.waveform      = add_param_env_carrier_waveform(model.plugin);
	model.params.env.modulator.pitch       = add_param_env_modulator_pitch(model.plugin);
	model.params.env.modulator.fm          = add_param_env_modulator_fm(model.plugin);
	model.params.env.noise.amount          = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_AMOUNT});
	model.params.env.noise.color           = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_COLOR});
	model.params.option.noise_mode         = blink::add::param::option(model.plugin, {BLINK_STD_UUID_NOISE_MODE});
	model.params.slider_real.noise_width   = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_NOISE_WIDTH});
	model.params.slider_real.amp           = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.slider_real.pan           = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.slider_real.carrier_pitch = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.amount, model.params.option.noise_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.amount, model.params.slider_real.noise_width);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.color, model.params.option.noise_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.color, model.params.slider_real.noise_width);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.amp, model.params.env.amp);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.pan, model.params.env.pan);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.carrier_pitch, model.params.env.carrier.pitch);
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
	dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(plugin);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::plugin::get_filesystem(), path);
}