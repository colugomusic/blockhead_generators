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
	out.requires_sample_analysis          = {false};
	return out;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid                = {"bd64e4c8-f788-433b-a42a-d375afd92503"};
	out.name                = {"Classic"};
	out.version             = {PLUGIN_VERSION};
	out.enable_warp_markers = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host, make_sampler_info());
	model.params.env.amp                 = blink::add::param::env(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.env.pan                 = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.env.pitch               = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.env.noise_amount        = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_AMOUNT});
	model.params.env.noise_color         = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_COLOR});
	model.params.option.loop             = blink::add::param::option(model.plugin, {BLINK_STD_UUID_LOOP});
	model.params.option.reverse_mode     = blink::add::param::option(model.plugin, {BLINK_STD_UUID_REVERSE_MODE});
	model.params.option.reverse_toggle   = blink::add::param::option(model.plugin, {BLINK_STD_UUID_REVERSE_TOGGLE});
	model.params.option.noise_mode       = blink::add::param::option(model.plugin, {BLINK_STD_UUID_NOISE_MODE});
	model.params.slider_real.noise_width = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_NOISE_WIDTH});
	model.params.slider_real.amp         = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.slider_real.pan         = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.slider_real.pitch       = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.slider_int.samp_offs    = blink::add::param::slider_int(model.plugin, {BLINK_STD_UUID_SAMPLE_OFFSET});
	blink::write::param::add_flags(model.plugin, model.params.env.amp, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.pitch, blink_ParamFlags_DefaultActive);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise_amount, model.params.option.noise_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise_amount, model.params.slider_real.noise_width);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise_color, model.params.option.noise_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise_color, model.params.slider_real.noise_width);
	blink::write::param::manip_delegate(model.plugin, model.params.option.reverse_toggle, model.params.option.reverse_mode);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.amp, model.params.env.amp);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.pan, model.params.env.pan);
	blink::write::param::manip_delegate(model.plugin, model.params.slider_real.pitch, model.params.env.pitch);
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

auto blink_sampler_draw(const blink_SamplerVaryingData* varying, const blink_SamplerUniformData* uniform, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	return draw(&model, *varying, *uniform, n, out);
}

auto blink_sampler_analyze_sample(void* host, blink_AnalysisCallbacks callbacks, const blink_SampleInfo* sample_info) -> blink_AnalysisResult {
	return blink_AnalysisResult_Error;
}

auto blink_sampler_sample_deleted(blink_ID sample_id) -> blink_Error {
	return blink_StdError_NotImplemented;
}

auto blink_sampler_process(blink_UnitIdx unit_idx, const blink_SamplerVaryingData* varying, const blink_SamplerUniformData* uniform, float* out) -> blink_Error {
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
	return BLINK_OK;
}

