#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

namespace resonator {
namespace convert {

inline auto linear_to_ratio(float x)
{
	return std::pow(4.0f, x);
}

inline auto linear_to_ratio(const ml::DSPVector& x)
{
	return ml::pow({4.0f}, x);
}

inline auto ratio_to_linear(float x)
{
	return std::log(x) / std::log(4.0f);
};

} // convert
} // resonator