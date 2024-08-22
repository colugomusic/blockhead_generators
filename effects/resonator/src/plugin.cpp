#define BLINK_EXPORT

#include "convert.h"
#include "dsp.hpp"
#include "model.h"
#include "shared/harmonics.hpp"
#include <blink_std.h>
#include <regex>

static Model model;

namespace tweak {
namespace fm_ratio {

[[nodiscard]]
auto constrain(float value) -> float {
	return std::clamp(value, -4.0f, 1.0f);
}

[[nodiscard]]
auto decrement(float value, bool precise) -> float {
	return blink::tweak::decrement<10, 100>(value, precise);
}

[[nodiscard]]
auto increment(float value, bool precise) -> float {
	return blink::tweak::increment<10, 100>(value, precise);
}

[[nodiscard]]
auto drag(float value, int amount, bool precise) -> float {
	return blink::tweak::drag<float, 10, 100>(value, amount / 5, precise);
}

[[nodiscard]]
auto stepify(float value) -> float {
	return convert::ratio_to_linear(blink::tweak::stepify<1000, float>(convert::linear_to_ratio(value)));
}

[[nodiscard]]
auto to_string(float value) -> std::string {
	constexpr auto threshold = 0.001f;
	const auto milestone_hit = [threshold](float value, float milestone) {
		return value > milestone - threshold && value < milestone + threshold;
	};
	value = convert::linear_to_ratio(value);
	if (milestone_hit(value, 2.0f)) {
		return "Double";
	} 
	if (milestone_hit(value, 3.0f)) {
		return "Triple";
	} 
	if (milestone_hit(value, 0.5f)) {
		return "Half";
	} 
	if (value < 1.0f) {
		const auto recip = 1.0f / value;
		const auto rounded_recip = std::round(recip); 
		if (std::abs(recip - rounded_recip) < threshold) {
			std::stringstream ss; 
			ss << "1/" << rounded_recip; 
			return ss.str();
		}
	}
	std::stringstream ss;
	ss << value << "x";
	return ss.str();
}

auto to_string(float value, char buffer[BLINK_STRING_MAX]) -> void {
	blink::tweak::write_string(to_string(value), buffer);
}

[[nodiscard]]
auto from_string(const char* c_str, float* out) -> blink_Bool {
	static constexpr auto HALF = convert::ratio_to_linear(0.5f);
	static constexpr auto DOUBLE = convert::ratio_to_linear(2.0f);
	static constexpr auto TRIPLE = convert::ratio_to_linear(3.0f);
	std::string str(c_str);
	std::string uppercase = str;
	std::transform(str.begin(), str.end(), uppercase.begin(), ::toupper);
	if (uppercase.find("HALF") != std::string::npos)   { *out = HALF; return {true}; }
	if (uppercase.find("DOUBLE") != std::string::npos) { *out = DOUBLE; return {true}; }
	if (uppercase.find("TRIPLE") != std::string::npos) { *out =TRIPLE; return {true}; }
	std::regex r("1/([\\d]+)");
	std::smatch match;
	if (std::regex_search(str, match, r)) {
		*out = 1.0f / std::stoi(match[1]);
		return {true};
	}
	const auto value = blink::tweak::find_number<float>(std::move(str));
	if (!value) {
		return {false};
	}
	*out = convert::ratio_to_linear(*value);
	return {true};
}

[[nodiscard]]
auto get_gridline(int index, float* out) -> blink_Bool {
	*out = float(index) * 0.5f;
	return {true};
}

[[nodiscard]]
auto fns() -> blink_EnvFns {
	blink_EnvFns fns = {0};
	fns.get_grid_line = get_gridline;
	fns.stepify       = stepify;
	fns.to_string     = to_string;
	return fns;
}

[[nodiscard]]
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain   = constrain;
	out.decrement   = decrement;
	out.increment   = increment;
	out.drag        = drag;
	out.from_string = from_string;
	out.stepify     = stepify;
	out.to_string   = to_string;
	return out;
}

} // fm_ratio
} // tweak

[[nodiscard]]
auto add_slider_fm_ratio(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, idx, 0.0f);
	blink::write::slider::tweaker(plugin, idx, tweak::fm_ratio::tweaker());
	return idx;
}

[[nodiscard]]
auto add_env_fm_ratio(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, -1.0f);
	blink::write::env::default_value(plugin, env_idx, 0.0f);
	blink::write::env::fns(plugin, env_idx, tweak::fm_ratio::fns());
	blink::write::env::value_slider(plugin, env_idx, add_slider_fm_ratio(plugin));
	return env_idx;
}

[[nodiscard]]
auto add_param_env_damper(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"94b001e1-0796-489a-a244-e3d6fcbcfb9c"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Damper"});
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::env::default_value(plugin, env_idx, 0.5f);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_fm_amount(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"d1b4d493-35f9-408b-982c-4474517a968c"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"FM Amount"});
	blink::write::param::short_name(plugin, param_idx, {"Amount"});
	blink::write::param::group(plugin, param_idx, {"FM"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 2.0f});
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_fm_ratio(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"25aac401-9251-4f88-af77-14dddba78151"});
	const auto env_idx   = add_env_fm_ratio(plugin);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_DefaultActive | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"FM Ratio"});
	blink::write::param::short_name(plugin, param_idx, {"Ratio"});
	blink::write::param::group(plugin, param_idx, {"FM"});
	blink::write::param::clamp_range(plugin, param_idx, {-4.0f, 1.0f});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	return param_idx;
}

[[nodiscard]]
auto add_param_env_harmonics_scale_snap_amount(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"de8c7343-360a-41fd-9788-ae2398d0a872"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::param::name(plugin, param_idx, {"Harmonics Scale Snap Amount"});
	blink::write::param::short_name(plugin, param_idx, {"Snap Amount"});
	blink::write::param::group(plugin, param_idx, {"Harmonics"});
	blink::write::param::long_desc(plugin, param_idx, {"Controls the amount of scale snapping for the harmonics"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, blink_ParamFlags_CanManipulate);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_harmonics_width(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"2f95dc68-9ae3-4e12-8ef1-81c36201b995"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::env::default_value(plugin, env_idx, 0.5f);
	blink::write::param::name(plugin, param_idx, {"Harmonics Width"});
	blink::write::param::short_name(plugin, param_idx, {"Width"});
	blink::write::param::group(plugin, param_idx, {"Harmonics"});
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
	out.uuid     = {"1e3f8aab-7f5c-40cc-9dc5-557d72256f43"};
	out.name     = {"Resonator"};
	out.category = {BLINK_STD_CATEGORY_TONAL};
	out.version  = {PLUGIN_VERSION};
	out.has_icon = {true};
	return out;
}

auto blink_init(blink_PluginIdx plugin_idx, blink_HostFns host) -> blink_Error {
	blink::init(&model.plugin, plugin_idx, host);
	model.params.env.pitch                       = blink::add::param::env(model.plugin, {BLINK_STD_UUID_PITCH});
	model.params.env.feedback                    = blink::add::param::env(model.plugin, {BLINK_STD_UUID_FEEDBACK});
	model.params.env.damper                      = add_param_env_damper(model.plugin);
	model.params.env.fm.amount                   = add_param_env_fm_amount(model.plugin);
	model.params.env.fm.ratio                    = add_param_env_fm_ratio(model.plugin);
	model.params.chord.harmonics.scale           = add::param::chord::harmonics_scale(model.plugin);
	model.params.env.harmonics.amount            = add::param::env::harmonics_amount(model.plugin);
	model.params.env.harmonics.spread            = add::param::env::harmonics_spread(model.plugin);
	model.params.env.harmonics.scale_snap_amount = add_param_env_harmonics_scale_snap_amount(model.plugin);
	model.params.env.harmonics.width             = add_param_env_harmonics_width(model.plugin);
	model.params.env.mix                         = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
	model.params.slider.pitch                    = blink::add::param::slider_real(model.plugin, {BLINK_STD_UUID_PITCH});
	blink::write::param::add_flags(model.plugin, model.params.env.feedback, blink_ParamFlags_DefaultActive);
	blink::write::param::add_flags(model.plugin, model.params.env.pitch, blink_ParamFlags_DefaultActive);
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
