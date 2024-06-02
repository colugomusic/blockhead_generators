#pragma once

#include <blink/plugin_impl.hpp>
#include <sstream>

namespace tract {
namespace convert {

auto linear_to_quality(float v) {
	return std::pow(2.0f, v - 1.0f) * 100.0f;
}

auto quality_to_linear(float v) {
	return std::log(0.02f * v) / std::log(2.0f);
}

} // convert

namespace tweak {
namespace quality {

auto constrain(float v) -> float {
	return std::clamp(v, 0.0f, 2.0f);
}

auto increment(float v, bool precise) -> float {
	return convert::quality_to_linear(blink::tweak::increment<1, 10>(convert::linear_to_quality(v), precise));
}

auto decrement(float v, bool precise) -> float {
	return convert::quality_to_linear(blink::tweak::decrement<1, 10>(convert::linear_to_quality(v), precise));
}

auto drag(float v, int amount, bool precise) -> float {
	return convert::quality_to_linear(blink::tweak::drag<float, 1, 10>(convert::linear_to_quality(v), amount / 5, precise));
}

auto to_string(float v, char buffer[BLINK_STRING_MAX]) -> void {
	std::stringstream ss;
	ss << std::pow(2.0f, v - 1.0f) * 100.0f << "%";
	blink::tweak::write_string(ss.str(), buffer);
}

auto from_string(char const* str, float* out) -> blink_Bool {
	auto value = blink::tweak::find_number<float>(std::move(str));
	if (!value) {
		return {false};
	}
	*out = convert::quality_to_linear(*value);
	return {true};
}

auto stepify(float v) -> float {
	return convert::quality_to_linear(blink::tweak::stepify<1>(convert::linear_to_quality(v)));
}

auto fns() -> blink_EnvFns {
	blink_EnvFns fns = {0};
	fns.stepify   = tweak::quality::stepify;
	fns.to_string = tweak::quality::to_string;
	return fns;
}

auto tweaker() -> blink_TweakerReal {
	blink_TweakerReal out = {0};
	return out;
}

} // quality
} // tweak

namespace add {
namespace env {

auto throat_diameter(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::percentage(plugin.host);
	blink_EnvFns fns = blink::tweak::percentage::fns();
	fns.get_grid_line = [](int index, float* out) -> blink_Bool {
		if (index == -1) {
			*out = 0.25f;
			return {true};
		}
		return {false};
	};
	fns.to_string = [](float value, char buffer[BLINK_STRING_MAX]) -> void {
		static constexpr auto CLOSED = 0.25f;
		static constexpr auto THRESHOLD = 0.001f;
		if (std::abs(value - CLOSED) < THRESHOLD) {
			blink::tweak::write_string("Closed", buffer);
			return;
		}
		blink::tweak::to_string(value, buffer);
	};
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::env::fns(plugin, env_idx, fns);
	return env_idx;
}

auto tongue_diameter(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::percentage(plugin.host);
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	return env_idx;
}

auto quality(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	const auto slider_max = blink::add::slider::empty_real(plugin.host);
	const auto slider_min = blink::add::slider::empty_real(plugin.host);
	const auto slider_value = blink::add::slider::empty_real(plugin.host);
	blink_TweakerReal tweaker = tweak::quality::tweaker();
	blink::write::env::default_max(plugin, env_idx, 2.0f);
	blink::write::env::default_min(plugin, env_idx, 0.0f);
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::env::fns(plugin, env_idx, tweak::quality::fns());
	blink::write::env::max_slider(plugin, env_idx, slider_max);
	blink::write::env::min_slider(plugin, env_idx, slider_min);
	blink::write::env::value_slider(plugin, env_idx, slider_value);
	blink::write::slider::default_value(plugin, slider_max, 2.0f);
	blink::write::slider::default_value(plugin, slider_min, 0.0f);
	blink::write::slider::default_value(plugin, slider_value, 1.0f);
	blink::write::slider::tweaker(plugin, slider_max, tweaker);
	blink::write::slider::tweaker(plugin, slider_min, tweaker);
	blink::write::slider::tweaker(plugin, slider_value, tweaker);
	return env_idx;
}

} // env

namespace param {
namespace env {

auto fricative_intensity(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"53c94f7f-d6b7-497e-8deb-9acd35f0d60e"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	const auto flags     = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::param::name(plugin, param_idx, {"Fricative Intensity"});
	blink::write::param::short_name(plugin, param_idx, {"Intensity"});
	blink::write::param::group(plugin, param_idx, {"Fricatives"});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

auto throat_diameter(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"ac0a8acd-29a4-42af-b75b-1d7ee2684b25"});
	const auto env_idx = add::env::throat_diameter(plugin);
	const auto flags = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Throat Diameter"});
	blink::write::param::short_name(plugin, param_idx, {"Diameter"});
	blink::write::param::group(plugin, param_idx, {"Throat"});
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::add_flags(plugin, param_idx, flags);
	return param_idx;
}

auto throat_position(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"64e246db-0d4d-4dc2-9d34-78a3a23070bc"});
	const auto env_idx   = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Throat Position"});
	blink::write::param::short_name(plugin, param_idx, {"Position"});
	blink::write::param::group(plugin, param_idx, {"Throat"});
	blink::write::param::clamp_range(plugin, param_idx, {-1.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

auto tongue_diameter(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"a14bfe06-b84d-4778-b50c-7290478cb6b3"});
	const auto env_idx   = add::env::tongue_diameter(plugin);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Tongue Diameter"});
	blink::write::param::short_name(plugin, param_idx, {"Diameter"});
	blink::write::param::group(plugin, param_idx, {"Tongue"});
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

auto tongue_position(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"1059abc4-3a1c-42cc-a21b-235b2c08775b"});
	const auto env_idx   = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags     = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Tongue Position"});
	blink::write::param::short_name(plugin, param_idx, {"Position"});
	blink::write::param::group(plugin, param_idx, {"Tongue"});
	blink::write::param::clamp_range(plugin, param_idx, {-1.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
	return param_idx;
}

auto quality(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"0111ffd7-78d1-4604-8d7b-956c34e280c3"});
	const auto env_idx = add::env::quality(plugin);
	const auto flags = blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Quality"});
	blink::write::param::long_desc(plugin, param_idx, {
		"When adjusted, the vocal tract model is run at a lower or higher sample rate "
		"(from 50% to 200%) which affects the perceived formant. A higher value uses more CPU."});
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, blink::add::env::percentage_bipolar(plugin.host));
	blink::write::param::override_env(plugin, param_idx, env_idx);
	blink::write::param::clamp_range(plugin, param_idx, {0.0f, 2.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::apply_offset_fn(plugin, param_idx, [](float value, float offset) { return value + (offset * 2); });
	return param_idx;
}

} // env
} // param
} // add
} // tract