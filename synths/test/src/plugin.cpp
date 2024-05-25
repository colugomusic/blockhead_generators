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

auto add_param_env_carrier_fm(const blink_HostFns& host) -> blink_ParamIdx {
	const auto idx = host.add_param_env(host.usr, {"05ffbc42-d2aa-4746-905e-44d4373b4345"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	host.write_param_group(host.usr, idx, {"Carrier"});
	host.write_param_name(host.usr, idx, {"Carrier FM Amount"});
	host.write_param_short_name(host.usr, idx, {"FM Amount"});
	host.write_param_add_flags(host.usr, idx, flags);
	host.write_param_env_env_idx(host.usr, idx, blink::add::env::percentage(host));
	host.write_param_env_clamp_range(host.usr, idx, {0.0f, 2.0f});
	host.write_param_env_offset_env(host.usr, idx, blink::add::env::percentage_bipolar(host));
	host.write_param_env_override_env(host.usr, idx, blink::add::env::percentage(host));
	return idx;
}

auto add_param_env_carrier_pitch(const blink_HostFns& host) -> blink_ParamIdx {
	const auto idx = host.add_param_env(host.usr, {BLINK_STD_UUID_PITCH});
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	host.write_param_group(host.usr, idx, {"Carrier"});
	host.write_param_name(host.usr, idx, {"Carrier Pitch"});
	host.write_param_short_name(host.usr, idx, {"Pitch"});
	host.write_param_add_flags(host.usr, idx, flags);
	return idx;
}

auto add_param_env_carrier_waveform(const blink_HostFns& host) -> blink_ParamIdx {
	const auto idx = host.add_param_env(host.usr, {"337b8f6f-1d7a-41a7-ab05-802b23887b7a"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	host.write_param_group(host.usr, idx, {"Carrier"});
	host.write_param_name(host.usr, idx, {"Waveform"});
	host.write_param_add_flags(host.usr, idx, flags);
	host.write_param_env_clamp_range(host.usr, idx, {0.0f, 1.0f});
	host.write_param_env_offset_env(host.usr, idx, blink::add::env::percentage_bipolar(host));
	host.write_param_env_override_env(host.usr, idx, blink::add::env::percentage(host));
	return idx;
}

auto add_param_env_modulator_fm(const blink_HostFns& host) -> blink_ParamIdx {
	const auto idx = host.add_param_env(host.usr, {"343689ff-d1bf-48f0-96bf-7a6689568ef5"});
	const auto flags = blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	host.write_param_group(host.usr, idx, {"Modulator"});
	host.write_param_name(host.usr, idx, {"Modulator FM Amount"});
	host.write_param_short_name(host.usr, idx, {"FM Amount"});
	host.write_param_add_flags(host.usr, idx, flags);
	host.write_param_env_env_idx(host.usr, idx, blink::add::env::percentage(host));
	host.write_param_env_clamp_range(host.usr, idx, {0.0f, 2.0f});
	host.write_param_env_offset_env(host.usr, idx, blink::add::env::percentage_bipolar(host));
	host.write_param_env_override_env(host.usr, idx, blink::add::env::percentage(host));
	return idx;
}

auto add_param_env_modulator_pitch(const blink_HostFns& host) -> blink_ParamIdx {
	const auto idx = host.add_param_env(host.usr, {BLINK_STD_UUID_PITCH});
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	host.write_param_group(host.usr, idx, {"Modulator"});
	host.write_param_name(host.usr, idx, {"Modulator Pitch"});
	host.write_param_short_name(host.usr, idx, {"Pitch"});
	host.write_param_add_flags(host.usr, idx, flags);
	return idx;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.amp                   = host.add_param_env(host.usr, {BLINK_STD_UUID_AMP});
	model.params.env.carrier.fm            = add_param_env_carrier_fm(host);
	model.params.env.carrier.pitch         = add_param_env_carrier_pitch(host);
	model.params.env.carrier.waveform      = add_param_env_carrier_waveform(host);
	model.params.env.modulator.fm          = add_param_env_modulator_fm(host);
	model.params.env.modulator.pitch       = add_param_env_modulator_pitch(host);
	model.params.env.noise.amount          = host.add_param_env(host.usr, {BLINK_STD_UUID_NOISE_AMOUNT});
	model.params.env.noise.color           = host.add_param_env(host.usr, {BLINK_STD_UUID_NOISE_COLOR});
	model.params.env.pan                   = host.add_param_env(host.usr, {BLINK_STD_UUID_PAN});
	model.params.option.noise_mode         = host.add_param_option(host.usr, {BLINK_STD_UUID_NOISE_MODE});
	model.params.slider_real.amp           = host.add_param_slider_real(host.usr, {BLINK_STD_UUID_AMP});
	model.params.slider_real.carrier_pitch = host.add_param_slider_real(host.usr, {BLINK_STD_UUID_PITCH});
	model.params.slider_real.noise_width   = host.add_param_slider_real(host.usr, {BLINK_STD_UUID_NOISE_WIDTH});
	model.params.slider_real.pan           = host.add_param_slider_real(host.usr, {BLINK_STD_UUID_PAN});
	host.write_param_add_flags(host.usr, model.params.env.amp, blink_ParamFlags_DefaultActive);
	host.write_param_add_subparam(host.usr, model.params.env.noise.amount, model.params.option.noise_mode);
	host.write_param_add_subparam(host.usr, model.params.env.noise.amount, model.params.slider_real.noise_width);
	host.write_param_add_subparam(host.usr, model.params.env.noise.color, model.params.option.noise_mode);
	host.write_param_add_subparam(host.usr, model.params.env.noise.color, model.params.slider_real.noise_width);
	host.write_param_manip_delegate(host.usr, model.params.slider_real.amp, model.params.env.amp);
	host.write_param_manip_delegate(host.usr, model.params.slider_real.pan, model.params.env.pan);
	host.write_param_manip_delegate(host.usr, model.params.slider_real.carrier_pitch, model.params.env.carrier.pitch);
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

auto blink_synth_process(blink_UnitIdx unit_idx, const blink_SynthBuffer* buffer, const blink_SynthUnitState* unit_state, float* out) -> blink_Error {
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

CMRC_DECLARE(test);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::test::get_filesystem(), path);
}