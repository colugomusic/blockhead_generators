#define BLINK_EXPORT

#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <algorithm>
#include <blink_std.h>
#include "convert.h"
#include "dsp.hpp"
#include "model.h"

static Model model;

namespace tweak {
namespace attack {

[[nodiscard]] inline
auto stepify(float x) -> float {
	x = convert::linear_to_attack(x);
	x = blink::tweak::stepify<1000>(x);
	x = convert::attack_to_linear(x);
	return x;
}

inline
auto to_string(float x, char buffer[BLINK_STRING_MAX]) -> void {
	std::stringstream ss;
	ss << blink::tweak::stepify<1000>(convert::linear_to_attack(x)) << " ms";
	blink::tweak::write_string(ss.str(), buffer);
}

[[nodiscard]] inline
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain = [](float x) -> float {
		return std::clamp(x, convert::attack_to_linear(ATTACK_MIN), convert::attack_to_linear(ATTACK_MAX));
	};
	out.decrement = [](float x, bool precise) -> float {
		x = convert::linear_to_attack(x);
		x -= precise ? 0.001f : 0.01f;
		return convert::attack_to_linear(x);
	};
	out.drag = [](float x, int amount, bool precise) -> float {
		x = convert::linear_to_attack(x);
		x += amount * (precise ? 0.001f : 0.01f);
		return convert::attack_to_linear(x);
	};
	out.from_string = [](const char* str, float* out) -> blink_Bool {
		const auto value = blink::tweak::find_positive_number<float>(str);
		if (value) {
			*out = convert::attack_to_linear(*value);
			return {true};
		}
		return {false};
	};
	out.increment = [](float x, bool precise) -> float {
		x = convert::linear_to_attack(x);
		x += precise ? 0.001f : 0.01f;
		return convert::attack_to_linear(x);
	};
	out.stepify   = stepify;
	out.to_string = to_string;
	return out;
}

} // attack

namespace release {

[[nodiscard]] inline
auto stepify(float x) -> float {
	x = convert::linear_to_release(x);
	x = blink::tweak::stepify<1000>(x);
	x = convert::release_to_linear(x);
	return x;
}

inline
auto to_string(float x, char buffer[BLINK_STRING_MAX]) -> void {
	std::stringstream ss;
	ss << blink::tweak::stepify<1000>(convert::linear_to_release(x)) << " ms";
	blink::tweak::write_string(ss.str(), buffer);
}

[[nodiscard]] inline
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain = [](float x) -> float {
		return std::clamp(x, convert::release_to_linear(ATTACK_MIN), convert::release_to_linear(ATTACK_MAX));
	};
	out.decrement = [](float x, bool precise) -> float {
		x = convert::linear_to_release(x);
		x -= precise ? 0.001f : 0.01f;
		return convert::release_to_linear(x);
	};
	out.drag = [](float x, int amount, bool precise) -> float {
		x = convert::linear_to_release(x);
		x += amount * (precise ? 0.001f : 0.01f);
		return convert::release_to_linear(x);
	};
	out.from_string = [](const char* str, float* out) -> blink_Bool {
		const auto value = blink::tweak::find_positive_number<float>(str);
		if (value) {
			*out = convert::release_to_linear(*value);
			return {true};
		}
		return {false};
	};
	out.increment = [](float x, bool precise) -> float {
		x = convert::linear_to_release(x);
		x += precise ? 0.001f : 0.01f;
		return convert::release_to_linear(x);
	};
	out.stepify   = stepify;
	out.to_string = to_string;
	return out;
}

} // release

namespace ratio {

[[nodiscard]] inline
auto constrain(float x) -> float {
	return std::clamp(x, 0.0f, 1.0f);
}

[[nodiscard]] inline
auto decrement(float x, bool precise) -> float {
	return blink::math::convert::ratio_to_linear(blink::tweak::decrement<1, 10>(blink::math::convert::linear_to_ratio(x), precise));
}

[[nodiscard]] inline
auto drag(float x, int amount, bool precise) -> float {
	return blink::math::convert::ratio_to_linear(blink::tweak::drag<float, 10, 100>(blink::math::convert::linear_to_ratio(x), amount / 5, precise));
}

[[nodiscard]] inline
auto increment(float x, bool precise) -> float {
	return blink::math::convert::ratio_to_linear(blink::tweak::increment<1, 10>(blink::math::convert::linear_to_ratio(x), precise));
}

[[nodiscard]] inline
auto stepify(float x) -> float {
	return blink::math::convert::ratio_to_linear(blink::tweak::stepify<1000>(blink::math::convert::linear_to_ratio(x)));
}

inline
auto to_string(float x, char buffer[BLINK_STRING_MAX]) -> void {
	std::stringstream ss;
	ss << blink::math::convert::linear_to_ratio(x) << " : 1";
	blink::tweak::write_string(ss.str(), buffer);
}

[[nodiscard]] inline
auto fns() -> blink_EnvFns {
	blink_EnvFns out = {0};
	out.get_grid_line = [](int index, float* out) -> blink_Bool {
		switch (index) {
			case 1: *out = blink::math::convert::ratio_to_linear(1.0f); return {true};
			case 2: *out = blink::math::convert::ratio_to_linear(2.0f); return {true};
			case 3: *out = blink::math::convert::ratio_to_linear(4.0f); return {true};
			case 4: *out = blink::math::convert::ratio_to_linear(8.0f); return {true};
			case 5: *out = blink::math::convert::ratio_to_linear(16.0f); return {true};
			case 6: *out = blink::math::convert::ratio_to_linear(32.0f); return {true};
			case 7: *out = blink::math::convert::ratio_to_linear(64.0f); return {true};
			default: return {false};
		}
	};
	out.stepify = stepify;
	out.to_string = to_string;
	return out;
}

[[nodiscard]] inline
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain = constrain;
	out.decrement = decrement;
	out.drag = drag;
	out.increment = increment;
	out.stepify = stepify;
	out.to_string = to_string;
	return out;
}

} // ratio
} // tweak

[[nodiscard]]
auto add_slider_attack(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, sld_idx, convert::attack_to_linear(ATTACK_DEFAULT));
	blink::write::slider::tweaker(plugin, sld_idx, tweak::attack::tweaker());
	return sld_idx;
}

[[nodiscard]]
auto add_slider_release(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, sld_idx, convert::release_to_linear(RELEASE_DEFAULT));
	blink::write::slider::tweaker(plugin, sld_idx, tweak::release::tweaker());
	return sld_idx;

}

[[nodiscard]]
auto add_slider_threshold(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink_TweakerReal tweaker = blink::tweak::amp::tweaker();
	tweaker.constrain = [](float x) -> float {
		const auto db = blink::math::convert::linear_to_db(x);
		if (db < -60.0f) return 0.0f;
		if (db > 0.0f) return 1.0f;
		return x;
	};
	blink::write::slider::default_value(plugin, sld_idx, blink::math::convert::db_to_linear(-20.0f));
	blink::write::slider::tweaker(plugin, sld_idx, tweaker);
	return sld_idx;
}

[[nodiscard]]
auto add_slider_ratio(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, sld_idx, blink::math::convert::ratio_to_linear(4.0f));
	blink::write::slider::tweaker(plugin, sld_idx, tweak::ratio::tweaker());
	return sld_idx;
}

[[nodiscard]]
auto add_env_attack(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink_EnvFns fns = {0};
	fns.stepify   = tweak::attack::stepify;
	fns.to_string = tweak::attack::to_string;
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::default_value(plugin, env_idx, convert::attack_to_linear(ATTACK_DEFAULT));
	blink::write::env::fns(plugin, env_idx, fns);
	blink::write::env::value_slider(plugin, env_idx, add_slider_attack(plugin));
	return env_idx;
}

[[nodiscard]]
auto add_env_release(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink_EnvFns fns = {0};
	fns.stepify   = tweak::release::stepify;
	fns.to_string = tweak::release::to_string;
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::default_value(plugin, env_idx, convert::release_to_linear(RELEASE_DEFAULT));
	blink::write::env::fns(plugin, env_idx, fns);
	blink::write::env::value_slider(plugin, env_idx, add_slider_release(plugin));
	return env_idx;
}

[[nodiscard]]
auto add_env_threshold(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink_EnvFns fns = {0};
	fns.stepify = blink::tweak::amp::stepify;
	fns.to_string = blink::tweak::amp::to_string;
	fns.get_grid_line = [](int index, float* out) -> blink_Bool {
		*out = blink::math::convert::linear_to_speed(float(index));
		return {true};
	};
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, blink::math::convert::db_to_linear(-60.0f));
	blink::write::env::default_value(plugin, env_idx, blink::math::convert::db_to_linear(-20.0f));
	blink::write::env::fns(plugin, env_idx, fns);
	blink::write::env::value_slider(plugin, env_idx, add_slider_threshold(plugin));
	return env_idx;
}

[[nodiscard]]
auto add_env_ratio(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	blink::write::env::default_value(plugin, env_idx, blink::math::convert::ratio_to_linear(4.0f));
	blink::write::env::default_max(plugin, env_idx, 1.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::fns(plugin, env_idx, tweak::ratio::fns());
	blink::write::env::value_slider(plugin, env_idx, add_slider_ratio(plugin));
	return env_idx;
}

[[nodiscard]]
auto add_param_env_attack(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"568d2cba-2fbb-4536-83d1-44781f8b0cab"});
	const auto env_idx   = add_env_attack(plugin);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Attack"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_release(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"990be907-d3f4-4b53-b65c-a5b4efb6ce97"});
	const auto env_idx   = add_env_release(plugin);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Release"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_threshold(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"dca075fe-a713-4f5e-b7c6-61c01f1e700b"});
	const auto env_idx   = add_env_threshold(plugin);
	const auto flags     = blink_ParamFlags_DefaultActive;
	blink::write::param::name(plugin, param_idx, {"Threshold"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_ratio(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"29bf39a4-b3c4-4a48-b577-dc252ce725c5"});
	const auto env_idx = add_env_ratio(plugin);
	const auto flags   = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Ratio"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

[[nodiscard]]
auto add_param_env_knee(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"7680370d-2c77-4b87-8079-a9890d84cc2f"});
	const auto env_idx   = blink::add::env::linear<0, 10, 1, 1>(plugin.host, 1.0f);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate;
	blink::write::param::name(plugin, param_idx, {"Knee"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

[[nodiscard]]
auto add_param_option_processing_mode(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::option(plugin, {"47601288-d3d0-4d35-9cd3-f2e985b49cde"});
	const auto flags = blink_ParamFlags_ShowButton;
	blink::write::param::name(plugin, param_idx, {"Processing Mode"});
	blink::write::param::strings(plugin, param_idx, {{"Mono", "Stereo"}});
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
	model.params.env.release = add_param_env_release(model.plugin);
	model.params.env.threshold = add_param_env_threshold(model.plugin);
	model.params.env.ratio = add_param_env_ratio(model.plugin);
	model.params.env.knee = add_param_env_knee(model.plugin);
	model.params.env.mix = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
	model.params.option.stereo = add_param_option_processing_mode(model.plugin);
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
	dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(plugin);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::plugin::get_filesystem(), path);
}
