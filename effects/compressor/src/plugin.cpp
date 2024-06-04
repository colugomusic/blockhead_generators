#define BLINK_EXPORT

#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <algorithm>
#include <blink_std.h>
#include <snd/ease.hpp>
#include <snd/misc.hpp>
#include "dsp.hpp"
#include "model.h"

static Model model;

static constexpr auto ATTACK_MIN = 0.001f;
static constexpr auto ATTACK_MAX = 10.f;

namespace convert {

template <typename T> [[nodiscard]] inline constexpr
auto linear_to_attack(T x) -> T {
	x = x * x * x * x;
	x = snd::lerp(ATTACK_MIN, ATTACK_MAX, x);
	return x;
}

template <typename T> [[nodiscard]] inline constexpr
auto attack_to_linear(T x) -> T {
	x = snd::inverse_lerp(ATTACK_MIN, ATTACK_MAX, x);
	x = blink::const_math::sqrt(blink::const_math::sqrt(x));
	return x;
}

static constexpr auto test = attack_to_linear(0.1f);

} // convert

namespace tweak {
namespace attack {

[[nodiscard]] inline
auto stepify(float x) -> float {
	x = convert::linear_to_attack(x);
	x = blink::tweak::stepify<1000>(x);
	x = convert::attack_to_linear(x);
	return x;
}

[[nodiscard]] inline
auto to_string(float x, char buffer[BLINK_STRING_MAX]) -> void {
	blink::tweak::to_string(blink::tweak::stepify<1000>(convert::linear_to_attack(x)), buffer);
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
} // tweak

[[nodiscard]]
auto add_slider_attack(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto sld_idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, sld_idx, convert::attack_to_linear(0.1f));
	blink::write::slider::tweaker(plugin, sld_idx, tweak::attack::tweaker());
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
	blink::write::env::default_value(plugin, env_idx, convert::attack_to_linear(0.1f));
	blink::write::env::fns(plugin, env_idx, fns);
	blink::write::env::value_slider(plugin, env_idx, add_slider_attack(plugin));
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

auto blink_get_error_string(blink_Error error) -> blink_TempString {
	return {blink::get_std_error_string(static_cast<blink_StdError>(error))};
}

auto blink_effect_get_info(blink_InstanceIdx instance_idx) -> blink_EffectInstanceInfo {
	return {0};
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
	//model.params.env.release = add_param_env_release(model.plugin);
	//model.params.env.threshold = add_param_env_threshold(model.plugin);
	//model.params.env.ratio = add_param_env_ratio(model.plugin);
	//model.params.env.knee = add_param_env_knee(model.plugin);
	model.params.env.mix = blink::add::param::env(model.plugin, {BLINK_STD_UUID_MIX});
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

auto blink_effect_process(blink_UnitIdx unit_idx, const blink_EffectBuffer* buffer, const blink_EffectUnitState* unit_state, const float* in, float* out) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	// TODO: return dsp::process(&model, &unit_dsp, *buffer, *unit_state, in, out);
	return {};
}

auto blink_terminate() -> blink_Error {
	return blink::terminate(&model.entities);
}

auto blink_unit_add(blink_InstanceIdx instance_idx) -> blink_UnitIdx {
	return blink::add_unit(&model.entities, instance_idx);
}

auto blink_unit_reset(blink_UnitIdx unit_idx) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	// TODO: dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

auto blink_unit_stream_init(blink_UnitIdx unit_idx, blink_SR SR) -> blink_Error {
	auto& unit_dsp = model.entities.unit.get<UnitDSP>(unit_idx.value);
	unit_dsp.SR = SR;
	// TODO: dsp::reset(&model, &unit_dsp);
	return BLINK_OK;
}

CMRC_DECLARE(compressor);

auto blink_get_resource_data(const char* path) -> blink_ResourceData {
	return blink::get_resource_data(&model.plugin, cmrc::compressor::get_filesystem(), path);
}