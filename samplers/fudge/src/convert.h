#pragma once

#include <cmath>
#include <blink/math.hpp>
#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

namespace convert {

constexpr auto GRAIN_SIZE_MIN_MS = 1.0f;
constexpr auto GRAIN_SIZE_MAX_MS = 1000.0f;

[[nodiscard]] inline
auto linear_to_ms(float linear) -> float {
	return blink::math::lerp(GRAIN_SIZE_MIN_MS, GRAIN_SIZE_MAX_MS, std::pow(linear, 4.0f));
}

[[nodiscard]] inline
auto ms_to_linear(float ms) -> float {
	return std::pow(blink::math::inverse_lerp(GRAIN_SIZE_MIN_MS, GRAIN_SIZE_MAX_MS, ms), 0.25f);
}

[[nodiscard]] inline
auto linear_to_ms(const ml::DSPVector& linear) -> ml::DSPVector {
	return ml::select(
		ml::DSPVector(GRAIN_SIZE_MIN_MS),
		ml::lerp(ml::DSPVector(GRAIN_SIZE_MIN_MS), ml::DSPVector(GRAIN_SIZE_MAX_MS), ml::pow(linear, ml::DSPVector(4.0f))),
		ml::lessThanOrEqual(linear, ml::DSPVector(0.0f)));
}

[[nodiscard]] inline
auto ms_to_linear(const ml::DSPVector& ms) -> ml::DSPVector {
	return ml::pow(ml::inverseLerp(ml::DSPVector(GRAIN_SIZE_MIN_MS), ml::DSPVector(GRAIN_SIZE_MAX_MS), ms), ml::DSPVector(0.25f));
}

[[nodiscard]] inline
auto ms_to_samples(const ml::DSPVector& ms, int SR) -> ml::DSPVector {
	return (ms / GRAIN_SIZE_MAX_MS) * float(SR);
}

} // convert