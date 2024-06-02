#pragma once

#include <blink/plugin_impl.hpp>

namespace tract {
namespace add {
namespace env {

auto diameter(const blink::Plugin& plugin) -> blink_EnvIdx {
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

auto quality(const blink::Plugin& plugin) -> blink_EnvIdx {
	const auto env_idx = blink::add::env::empty(plugin.host);
	// TODO:
	return env_idx;
}

/*
inline auto envelope()
{
	blink::EnvelopeSpec out;

	auto linear_to_quality = [](float v)
	{
		return std::pow(2.0f, v - 1.0f) * 100.0f;
	};

	auto quality_to_linear = [](float v)
	{
		return std::log(0.02f * v) / std::log(2.0f);
	};

	auto to_string = [](float v)
	{
		std::stringstream ss;

		ss << std::pow(2.0f, v - 1.0f) * 100.0f << "%";

		return ss.str();
	};

	auto from_string = [=](std::string str) -> ::std::optional<float>
	{
		auto value = tweak::find_number<float>(std::move(str));

		if (!value) return ::std::optional<float>();

		return quality_to_linear(*value);
	};

	auto stepify = [=](float v)
	{
		return quality_to_linear(tweak::math::stepify<1>(linear_to_quality(v)));
	};

	out.default_value = 1.0f;
	out.range.min.default_value = 0.0f;
	out.range.max.default_value = 2.0f;
	out.range.min.to_string = to_string;
	out.range.max.to_string = to_string;
	out.searcher.binary = blink::std_params::search::float_points_binary;
	out.searcher.forward = blink::std_params::search::float_points_forward;
	out.stepify = stepify;
	out.to_string = to_string;

	out.value_slider.constrain = [](float v)
	{
		return std::clamp(v, 0.0f, 2.0f);
	};

	out.value_slider.increment = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::increment<1, 10>(linear_to_quality(v), precise));
	};

	out.value_slider.decrement = [=](float v, bool precise)
	{
		return quality_to_linear(tweak::decrement<1, 10>(linear_to_quality(v), precise));
	};

	out.value_slider.drag = [=](float v, int amount, bool precise)
	{
		return quality_to_linear(tweak::drag<float, 1, 10>(linear_to_quality(v), amount / 5, precise));
	};

	out.value_slider.to_string = to_string;
	out.value_slider.from_string = from_string;
	out.value_slider.default_value = 1.0f;

	return out;
}
*/

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
	const auto env_idx = add::env::diameter(plugin);
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