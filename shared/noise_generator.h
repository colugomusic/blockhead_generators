#pragma once

#include <blink.h>
#include <blink/block_positions.hpp>
#include <blink/envelope_data.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class NoiseGenerator
{
public:

	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		float mode,
		const ml::DSPVector& amount,
		float color,
		float width);

	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		float mode,
		const blink::EnvelopeIndexData& env_amount,
		const blink::EnvelopeIndexData& env_color,
		float width,
		const blink::BlockPositions& block_positions);

private:

	ml::NoiseGen generator_;
	ml::OnePole filter_;
};