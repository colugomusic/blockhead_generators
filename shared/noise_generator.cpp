#include "noise_generator.h"

ml::DSPVectorArray<2> NoiseGenerator::operator()(const ml::DSPVectorArray<2>& in, Mode mode, const ml::DSPVector& amount, float color, float width)
{
	ml::DSPVectorArray<2> out;

	color = (color + 1.0f) * 0.5f;

	filter_.mCoeffs = ml::OnePole::coeffs(0.001f + (std::pow(color, 2.0f) * 0.6f));

	const auto noise_L = filter_(generator_());

	ml::DSPVectorArray<2> noise;

	if (width > 0.0f)
	{
		const auto noise_R = filter_(generator_());

		noise = ml::concatRows(noise_L, ml::lerp(noise_L, noise_R, width));
	}
	else
	{
		noise = ml::repeatRows<2>(noise_L);
	}

	if (mode == Mode::Multiply)
	{
		return in * ml::lerp(ml::DSPVectorArray<2>(1.0f), noise, ml::repeatRows<2>(amount));
	}
	else
	{
		return ml::lerp(in, noise, ml::repeatRows<2>(amount));
	}
}

ml::DSPVectorArray<2> NoiseGenerator::operator()(
	const ml::DSPVectorArray<2>& in,
	int mode,
	const blink::EnvelopeIndexData& env_amount,
	const blink::EnvelopeIndexData& env_color,
	float width,
	const blink::BlockPositions& block_positions)
{
	constexpr auto MIN_AMOUNT = 0.0001f;

	if (env_amount.data->points.count < 1)
	{
		return in;
	}

	const auto amount = env_amount.search_vec(block_positions);

	if (ml::sum(amount) < MIN_AMOUNT)
	{
		return in;
	}

	float color;

	env_color.search_vec(block_positions, 1, &color);

	return operator()(in, Mode(mode), amount, color, width);
}