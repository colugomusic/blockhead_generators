#pragma once

#include <blink/tweak.hpp>
#include <blink_std.h>

namespace tweak {
namespace harmonics_amount {

[[nodiscard]] inline
auto constrain(const float value) -> float {
	return blink::tweak::constrain(value, 0.0f, 3.0f);
}

[[nodiscard]] inline
auto constrain_offset(const float value) -> float {
	return blink::tweak::constrain(value, -3.0f, 3.0f);
}

[[nodiscard]] inline
auto drag(float value, int amount, bool precise) -> float {
	return blink::tweak::drag<float, 10, 100>(value, amount / 5, precise);
}

[[nodiscard]] inline
auto fns() -> blink_EnvFns {
	blink_EnvFns fns = {0};
	fns.get_grid_line = [](int index, float* out) -> blink_Bool {
		*out = float(index);
		return {true};
	};
	fns.stepify = &blink::tweak::stepify<100>;
	fns.to_string = blink::tweak::to_string;
	return fns;
}

[[nodiscard]] inline
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain   = &constrain;
	out.stepify     = &blink::tweak::stepify<100>;
	out.increment   = &blink::tweak::increment<1, 10>;
	out.decrement   = &blink::tweak::decrement<1, 10>;
	out.drag        = &drag;
	out.from_string = blink::tweak::positive_number_from_string;
	out.to_string   = blink::tweak::to_string;
	return out;
}

[[nodiscard]] inline
auto tweaker_offset() -> blink_TweakerReal {
	blink_TweakerReal out = tweaker();
	out.constrain = &constrain_offset;
	return out;
}

} // harmonics_amount

namespace harmonics_spread {

[[nodiscard]] inline
auto constrain(const float value) -> float {
	return blink::tweak::constrain(value, 0.0f, 4.0f);
}

[[nodiscard]] inline
auto drag(float value, int amount, bool precise) -> float {
	return blink::tweak::drag<float, 100, 1000>(value, amount / 5, precise);
}

[[nodiscard]] inline
auto fns() -> blink_EnvFns {
	blink_EnvFns fns = {0};
	fns.get_grid_line = [](int index, float* out) -> blink_Bool {
		*out = float(index) / 4.0f;
		return {true};
	};
	fns.stepify = blink::tweak::stepify<100>;
	fns.to_string = blink::tweak::percentage::to_string;
	return fns;
}

[[nodiscard]] inline
auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	out.constrain   = &constrain;
	out.increment   = blink::tweak::increment<100, 1000>;
	out.decrement   = blink::tweak::decrement<100, 1000>;
	out.drag        = &drag;
	out.from_string = blink::tweak::positive_number_from_string;
	out.stepify     = blink::tweak::percentage::stepify;
	out.to_string   = blink::tweak::to_string;
	return out;
}

} // harmonics_spread
} // tweak

namespace add {

namespace slider {

[[nodiscard]] inline
auto harmonics_amount(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, idx, 0.0f);
	blink::write::slider::tweaker(plugin, idx, tweak::harmonics_amount::tweaker());
	return idx;
}

[[nodiscard]] inline
auto harmonics_amount_offset(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, idx, 0.0f);
	blink::write::slider::tweaker(plugin, idx, tweak::harmonics_amount::tweaker_offset());
	return idx;
}

[[nodiscard]] inline
auto harmonics_spread(const blink::Plugin& plugin) -> blink_SliderRealIdx {
	const auto idx = blink::add::slider::empty_real(plugin.host);
	blink::write::slider::default_value(plugin, idx, 1.0f);
	blink::write::slider::tweaker(plugin, idx, tweak::harmonics_spread::tweaker());
	return idx;
}

} // slider

namespace env {

[[nodiscard]] inline
auto harmonics_amount(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto idx = blink::add::env::empty(plugin.host);
	blink::write::env::default_max(plugin, idx, 3.0f);
	blink::write::env::default_min(plugin, idx, 0.0f);
	blink::write::env::default_value(plugin, idx, 0.0f);
	blink::write::env::fns(plugin, idx, tweak::harmonics_amount::fns());
	blink::write::env::value_slider(plugin, idx, add::slider::harmonics_amount(plugin));
	return idx;
}

[[nodiscard]] inline
auto harmonics_amount_offset(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto idx = blink::add::env::empty(plugin.host);
	blink::write::env::default_max(plugin, idx, 3.0f);
	blink::write::env::default_min(plugin, idx, -3.0f);
	blink::write::env::default_value(plugin, idx, 0.0f);
	blink::write::env::fns(plugin, idx, tweak::harmonics_amount::fns());
	blink::write::env::value_slider(plugin, idx, add::slider::harmonics_amount_offset(plugin));
	return idx;
}

[[nodiscard]] inline
auto harmonics_spread(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	const auto sld_idx = add::slider::harmonics_spread(plugin);
	blink::write::env::default_max(plugin, env_idx, 2.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::env::fns(plugin, env_idx, tweak::harmonics_spread::fns());
	blink::write::env::value_slider(plugin, env_idx, sld_idx);
	blink::write::env::max_slider(plugin, env_idx, sld_idx);
	return env_idx;
}

} // env

namespace param {
namespace chord {

[[nodiscard]] inline
auto harmonics_scale(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto idx = blink::add::param::chord(plugin, {BLINK_STD_UUID_SCALE});
	const auto flags = blink_ParamFlags_CanManipulate | blink_ParamFlags_IconOnly | blink_ParamFlags_AlwaysShowWhenGroupVisible;
	blink::write::param::add_flags(plugin, idx, flags);
	blink::write::param::group(plugin, idx, {"Harmonics"});
	return idx;
}

} // chord

namespace env {

[[nodiscard]] static
auto harmonics_amount(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"a8a6e4fa-6713-48bb-a888-65ac437384b7"});
	const auto env_idx   = add::env::harmonics_amount(plugin);
	const auto flags = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Harmonics Amount"});
	blink::write::param::short_name(plugin, param_idx, {"Amount"});
	blink::write::param::group(plugin, param_idx, {"Harmonics"});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 3.0f});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, add::env::harmonics_amount_offset(plugin));
	return param_idx;
}

[[nodiscard]] static
auto harmonics_spread(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"f04c18fd-0341-4398-a02b-f3e253f658c1"});
	const auto env_idx   = add::env::harmonics_spread(plugin);
	const auto flags = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Harmonics Spread"});
	blink::write::param::short_name(plugin, param_idx, {"Spread"});
	blink::write::param::group(plugin, param_idx, {"Harmonics"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 10.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

} // env
} // param
} // add
