#pragma once

#include <blink/math.hpp>
#include <snd/ease.hpp>
#include <snd/misc.hpp>

static constexpr auto ATTACK_MIN      = 0.001f;
static constexpr auto ATTACK_MAX      = 10.f;
static constexpr auto ATTACK_DEFAULT  = 0.1f;
static constexpr auto RELEASE_MIN     = 0.001f;
static constexpr auto RELEASE_MAX     = 100.f;
static constexpr auto RELEASE_DEFAULT = 10.0f;

namespace convert {

template <typename T> [[nodiscard]] inline constexpr
auto linear_to_attack(T x) -> T {
	x = x * x * x * x;
	x = snd::lerp(ATTACK_MIN, ATTACK_MAX, x);
	return x;
}

template <typename T> [[nodiscard]] inline constexpr
auto linear_to_release(T x) -> T {
	x = x * x * x * x;
	x = snd::lerp(RELEASE_MIN, RELEASE_MAX, x);
	return x;
}

template <typename T> [[nodiscard]] inline constexpr
auto attack_to_linear(T x) -> T {
	x = snd::inverse_lerp(ATTACK_MIN, ATTACK_MAX, x);
	x = blink::const_math::sqrt(blink::const_math::sqrt(x));
	return x;
}

template <typename T> [[nodiscard]] inline constexpr
auto release_to_linear(T x) -> T {
	x = snd::inverse_lerp(RELEASE_MIN, RELEASE_MAX, x);
	x = blink::const_math::sqrt(blink::const_math::sqrt(x));
	return x;
}

} // convert
