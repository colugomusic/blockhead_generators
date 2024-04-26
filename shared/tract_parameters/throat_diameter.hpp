#pragma once

#include <blink/standard_parameters/percentage.hpp>

namespace tract_params {
namespace throat_diameter {

static constexpr auto UUID { "ac0a8acd-29a4-42af-b75b-1d7ee2684b25" };

inline auto envelope()
{
	auto out { blink::std_params::percentage::envelope() };

	out.default_value = 1.0f;
	out.get_gridline = [](int index) -> std::optional<float>
	{
		if (index == -1) return 0.25f;

		return std::optional<float>();
	};

	out.to_string = [](float value) -> std::string
	{
		static constexpr auto CLOSED = 0.25f;
		static constexpr auto THRESHOLD = 0.001f;

		if (std::abs(value - CLOSED) < THRESHOLD)
		{
			return "Closed";
		}

		return tweak::to_string(value);
	};

	return out;
}

inline auto envelope_parameter()
{
	blink::EnvelopeParameterSpec out;

	out.uuid = UUID;
	out.name = "Throat Diameter";
	out.short_name = "Diameter";
	out.envelope = envelope();
	out.clamp_range = { 0.0f, 1.0f };

	out.flags |= blink_ParamFlags_DefaultActive;
	out.flags |= blink_ParamFlags_CanManipulate;
	out.flags |= blink_ParamFlags_HostClamp;

	out.offset_envelope = blink::std_params::percentage::bipolar::envelope();
	out.override_envelope = envelope();

	return out;
}

} // throat_diameter
} // tract_params
