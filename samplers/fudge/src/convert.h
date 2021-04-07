#pragma once

#include <cmath>
#include "limits.h"
#include <blink/math.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

namespace convert {

inline float linear_to_ms(float linear)
{
	return blink::math::lerp(limits::GRAIN_SIZE_MIN_MS, limits::GRAIN_SIZE_MAX_MS, std::pow(linear, 4.0f));
}

inline float ms_to_linear(float ms)
{
	return std::pow(blink::math::inverse_lerp(limits::GRAIN_SIZE_MIN_MS, limits::GRAIN_SIZE_MAX_MS, ms), 0.25f);
}

inline ml::DSPVector linear_to_ms(const ml::DSPVector& linear)
{
	return ml::select(
		ml::DSPVector(limits::GRAIN_SIZE_MIN_MS),
		ml::lerp(ml::DSPVector(limits::GRAIN_SIZE_MIN_MS), ml::DSPVector(limits::GRAIN_SIZE_MAX_MS), ml::pow(linear, ml::DSPVector(4.0f))),
		ml::lessThanOrEqual(linear, ml::DSPVector(0.0f)));
}

inline ml::DSPVector ms_to_linear(const ml::DSPVector& ms)
{
	return ml::pow(ml::inverseLerp(ml::DSPVector(limits::GRAIN_SIZE_MIN_MS), ml::DSPVector(limits::GRAIN_SIZE_MAX_MS), ms), ml::DSPVector(0.25f));
}

inline ml::DSPVector ms_to_samples(const ml::DSPVector& ms, int SR)
{
	return (ms / limits::GRAIN_SIZE_MAX_MS) * float(SR);
}

}