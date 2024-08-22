#define BLINK_EXPORT

#include <blink_std.h>
#include <blink/math.hpp>
#include "convert.h"
#include "draw.hpp"
#include "dsp.hpp"
#include "model.h"
#include "shared/harmonics.hpp"

static Model model;

namespace tweak {
namespace grain_size {

[[nodiscard]] static
auto snap_value(float v, float step_size, float snap_amount) -> float {
	return blink::tweak::snap_value(v, step_size, snap_amount);
}

[[nodiscard]] static
auto constrain(float v) -> float {
	return std::clamp(v, 0.0f, 1.0f);
};

[[nodiscard]] static
auto increment(float v, bool precise) {
	return blink::tweak::increment<100, 1000>(v, precise);
}

[[nodiscard]] static
auto decrement(float v, bool precise) {
	return blink::tweak::decrement<100, 1000>(v, precise);
}

[[nodiscard]] static
auto drag(float v, int amount, bool precise) -> float {
	return blink::tweak::drag<float, 100, 1000>(v, amount / 5, precise);
}

static
auto to_string(float v, char buffer[BLINK_STRING_MAX]) -> void {
	std::stringstream ss;
	ss << convert::linear_to_ms(v) << " ms";
	blink::tweak::write_string(ss.str(), buffer);
}

[[nodiscard]] static
auto from_string(const char* str, float* out) -> blink_Bool {
	auto value = blink::tweak::find_number<float>(std::move(str));
	if (!value) {
		return {false};
	}
	*out = convert::ms_to_linear(*value);
	return {true};
}

[[nodiscard]] static
auto env_fns() -> blink_EnvFns {
	blink_EnvFns out = {0};
	out.snap_value = tweak::grain_size::snap_value;
	out.to_string  = tweak::grain_size::to_string;
	return out;
}

[[nodiscard]] static
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain   = tweak::grain_size::constrain;
	out.decrement   = tweak::grain_size::decrement;
	out.drag        = tweak::grain_size::drag;
	out.from_string = tweak::grain_size::from_string;
	out.increment   = tweak::grain_size::increment;
	out.to_string   = tweak::grain_size::to_string;
	return out;
}

} // grain_size
} // tweak

[[nodiscard]] static
auto add_param_env_amp(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_AMP});
	blink::write::param::add_flags(plugin, idx, blink_ParamFlags_DefaultActive);
	return idx;
}

[[nodiscard]] static
auto add_param_env_pitch(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::add_flags(plugin, idx, blink_ParamFlags_DefaultActive);
	return idx;
}

[[nodiscard]] static
auto add_slider_grain_size(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, sld_idx, 0.5f);
	blink::write::slider::tweaker(plugin, sld_idx, tweak::grain_size::tweaker());
	return sld_idx;
}

[[nodiscard]] static
auto add_env_grain_size(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::default_value(plugin, env_idx, 0.5f);
	blink::write::env::fns(plugin, env_idx, ::tweak::grain_size::env_fns());
	blink::write::env::value_slider(plugin, env_idx, add_slider_grain_size(plugin));
	return env_idx;
}

[[nodiscard]] static
auto add_param_env_grain_size(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"2a9c7ec6-68b4-40c1-aad9-b3c010cd9717"});
	const auto env_idx   = add_env_grain_size(plugin);
	const auto flags = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp | blink_ParamFlags_DefaultActive;
	blink::write::param::name(plugin, param_idx, {"Grain Size"});
	blink::write::param::short_name(plugin, param_idx, {"Size"});
	blink::write::param::group(plugin, param_idx, {"Geometry"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

[[nodiscard]] static
auto add_param_env_grain_transpose(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::uuid(plugin, idx, {"fe64baa2-9d35-4eef-bc8b-1799916e8bda"});
	blink::write::param::name(plugin, idx, {"Grain Transpose"});
	blink::write::param::short_name(plugin, idx, {"Transpose"});
	blink::write::param::group(plugin, idx, {"Geometry"});
	return idx;
}

[[nodiscard]] static
auto add_param_env_grain_uniformity(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"83c352fb-35b5-4cb0-a6f7-05d082b56a16"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Grain Uniformity"});
	blink::write::param::short_name(plugin, param_idx, {"Uniformity"});
	blink::write::param::group(plugin, param_idx, {"Geometry"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

[[nodiscard]] static
auto add_param_env_speed(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::env(plugin, {BLINK_STD_UUID_SPEED});
	blink::write::param::add_flags(plugin, idx, blink_ParamFlags_DefaultActive);
	return idx;
}

[[nodiscard]] static
auto make_sampler_info() -> blink_SamplerInfo {
	blink_SamplerInfo out = {0};
	out.baked_waveform_could_be_different = {false};
	out.requires_sample_analysis          = {true};
	return out;
}

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_get_plugin_info() -> blink_PluginInfo {
	blink_PluginInfo out = {0};
	out.uuid                = {"795c8dc6-3b81-4397-abac-071bab83b10f"};
	out.name                = {"Fudge"};
	out.version             = {PLUGIN_VERSION};
	out.enable_warp_markers = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host, make_sampler_info());
	model.params.env.amp               = add_param_env_amp(model.plugin);
	model.params.env.pan               = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.env.pitch             = add_param_env_pitch(model.plugin);
	model.params.env.speed             = add_param_env_speed(model.plugin);
	model.params.env.grain.size        = add_param_env_grain_size(model.plugin);
	model.params.env.grain.transpose   = add_param_env_grain_transpose(model.plugin);
	model.params.env.grain.uniformity  = add_param_env_grain_uniformity(model.plugin);
	model.params.chord.harmonics_scale = add::param::chord::harmonics_scale(model.plugin);
	model.params.env.harmonics.amount  = add::param::env::harmonics_amount(model.plugin);
	model.params.env.harmonics.spread  = add::param::env::harmonics_spread(model.plugin);
	model.params.env.noise.amount      = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_AMOUNT});
	model.params.env.noise.color       = blink::add::param::env(model.plugin, {BLINK_STD_UUID_NOISE_COLOR});
	model.params.slider.amp            = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_AMP});
	model.params.slider.pan            = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PAN});
	model.params.slider.pitch          = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.slider.speed          = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_SPEED});
	model.params.slider.noise_width    = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_NOISE_WIDTH});
	model.params.slider.sample_offset  = blink::add::param::slider_int(model.plugin, {BLINK_STD_UUID_SAMPLE_OFFSET});
	model.params.option.noise_mode     = blink::add::param::option(model.plugin, {BLINK_STD_UUID_NOISE_MODE});
	model.params.option.reverse_mode   = blink::add::param::option(model.plugin, {BLINK_STD_UUID_REVERSE_MODE});
	model.params.toggle.loop           = blink::add::param::option(model.plugin, {BLINK_STD_UUID_LOOP});
	model.params.toggle.reverse        = blink::add::param::option(model.plugin, {BLINK_STD_UUID_REVERSE_TOGGLE});
	blink::write::param::manip_delegate(model.plugin, model.params.slider.amp, model.params.env.amp);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.pan, model.params.env.pan);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.pitch, model.params.env.pitch);
	blink::write::param::manip_delegate(model.plugin, model.params.slider.speed, model.params.env.speed);
	blink::write::param::manip_delegate(model.plugin, model.params.toggle.reverse, model.params.option.reverse_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.amount, model.params.slider.noise_width);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.amount, model.params.option.noise_mode);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.color, model.params.slider.noise_width);
	blink::write::param::add_subparam(model.plugin, model.params.env.noise.color, model.params.option.noise_mode);
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

auto blink_sampler_analyze_sample(void* host, blink_AnalysisCallbacks callbacks, const blink_SampleInfo* sample_info) -> blink_AnalysisResult {
	auto pos = model.sample_analysis.find(sample_info->id);
	if (pos != model.sample_analysis.end()) {
		return blink_AnalysisResult_OK;
	}
	auto analysis = std::make_shared<SampleAnalysis>();
	const auto result = analyze(host, callbacks, *sample_info, analysis.get());
	if (result == blink_AnalysisResult_OK) {
		model.sample_analysis[sample_info->id] = analysis;
	}
	return result;
}

auto blink_sampler_sample_deleted(blink_ID sample_id) -> blink_Error {
	const auto pos = model.sample_analysis.find(sample_id);
	if (pos == model.sample_analysis.end()) return BLINK_OK;
	model.sample_analysis.erase(pos);
	return BLINK_OK;
}

auto blink_sampler_draw(const blink_SamplerVaryingData* varying, const blink_SamplerUniformData* uniform, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	return draw(&model, *varying, *uniform, n, out);
}

auto blink_sampler_process(blink_UnitIdx unit_idx, const blink_SamplerVaryingData* varying, const blink_SamplerUniformData* uniform, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	return dsp::process(&model, &unit_dsp, *varying, *uniform, out);
}

auto blink_terminate() -> blink_Error {
	return blink::terminate(&model.entities);
}

auto blink_unit_add(blink_InstanceIdx instance_idx) -> blink_UnitIdx {
	const auto idx = blink::add_unit(&model.entities, instance_idx);
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(idx.value);
	dsp::init(&model, &unit_dsp);
	return idx;
}

auto blink_unit_reset(blink_UnitIdx unit_idx) -> blink_Error {
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	dsp::init(&model, &unit_dsp);
	return BLINK_OK;
}
