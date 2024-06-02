#pragma once

#include <blink/plugin_impl.hpp>

namespace tract {

auto add_env_diameter(const blink::Plugin& plugin) -> blink_EnvIdx {
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

auto add_param_env_throat_diameter(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"ac0a8acd-29a4-42af-b75b-1d7ee2684b25"});
	const auto env_idx = add_env_diameter(plugin);
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

auto add_param_env_throat_position(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"64e246db-0d4d-4dc2-9d34-78a3a23070bc"});
	const auto env_idx   = blink::add::env::percentage_bipolar(plugin.host);
	const auto flags    = blink_ParamFlags_DefaultActive | blink_ParamFlags_CanManipulate | blink_ParamFlags_HostClamp;
	blink::write::param::name(plugin, param_idx, {"Throat Position"});
	blink::write::param::short_name(plugin, param_idx, {"Position"});
	blink::write::param::group(plugin, param_idx, {"Throat"});
	blink::write::param::clamp_range(plugin, param_idx, {-1.0f, 1.0f});
	blink::write::param::add_flags(plugin, param_idx, flags);
	blink::write::param::env(plugin, param_idx, env_idx);
	blink::write::param::offset_env(plugin, param_idx, env_idx);
	blink::write::param::override_env(plugin, param_idx, env_idx);
}

auto add_param_env_fricative_intensity(const blink::Plugin& plugin) -> blink_ParamIdx {
	const auto param_idx = blink::add::param::env(plugin, {"53c94f7f-d6b7-497e-8deb-9acd35f0d60e"});
	const auto env_idx   = blink::add::env::percentage(plugin.host);
	blink::write::env::default_value(plugin, env_idx, 1.0f);
	blink::write::param::group(plugin, param_idx, {"Fricatives"});
	// TODO:
	return param_idx;

	//inline auto envelope_parameter()
	//{
	//	blink::EnvelopeParameterSpec out;

	//	out.uuid = UUID;
	//	out.name = "Fricative Intensity";
	//	out.short_name = "Intensity";
	//	out.envelope = envelope();
	//	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	//	out.override_envelope = envelope();

	//	out.clamp_range = {0.0f, 1.0f};

	//	out.flags |= blink_ParamFlags_CanManipulate;
	//	out.flags |= blink_ParamFlags_HostClamp;

	//	return out;
	//}
}

} // tract