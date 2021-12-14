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

	enum class Mode
	{
		Multiply = 0,
		Mix = 1,
	};

	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		Mode mode,
		const ml::DSPVector& amount,
		float color,
		float width);

	ml::DSPVectorArray<2> operator()(
		const ml::DSPVectorArray<2>& in,
		int mode,
		const blink::EnvelopeIndexData& env_amount,
		const blink::EnvelopeIndexData& env_color,
		const blink_SliderData& data_sld_width,
		const blink::BlockPositions& block_positions);

private:

	ml::NoiseGen generator_;
	ml::OnePole filter_;
};