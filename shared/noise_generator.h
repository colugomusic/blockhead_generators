#pragma once

#include <blink/plugin_impl.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

struct NoiseGenerator {
	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		float mode,
		const ml::DSPVector& amount,
		float color,
		float width);
	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		float mode,
		const blink::uniform::Env& env_amount,
		const blink::uniform::Env& env_color,
		float width,
		const blink::BlockPositions& block_positions);
private:
	ml::NoiseGen generator_;
	ml::OnePole filter_;
};