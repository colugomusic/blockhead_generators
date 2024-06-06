#include <blink/search.hpp>
#include "noise_generator.h"

ml::DSPVectorArray<2> NoiseGenerator::operator()(const ml::DSPVectorArray<2>& in, float mode, const ml::DSPVector& amount, float color, float width) {
	ml::DSPVectorArray<2> out; 
	color = (color + 1.0f) * 0.5f; 
	filter_.mCoeffs = ml::OnePole::coeffs(0.001f + (std::pow(color, 2.0f) * 0.6f)); 
	const auto noise_L = filter_(generator_()); 
	ml::DSPVectorArray<2> noise; 
	if (width > 0.0f) {
		const auto noise_R = filter_(generator_()); 
		noise = ml::concatRows(noise_L, ml::lerp(noise_L, noise_R, width));
	}
	else {
		noise = ml::repeatRows<2>(noise_L);
	} 
	const auto mult { in * ml::lerp(ml::DSPVectorArray<2>(1.0f), noise, ml::repeatRows<2>(amount)) };
	const auto mix { ml::lerp(in, noise, ml::repeatRows<2>(amount)) }; 
	return ml::lerp(mult, mix, mode);
}

ml::DSPVectorArray<2> NoiseGenerator::operator()(
	const ml::DSPVectorArray<2>& in,
	float mode, // 0: multiply, 1: mix
	const blink::uniform::Env& env_amount,
	const blink::uniform::Env& env_color,
	float width,
	const blink::BlockPositions& block_positions)
{
	constexpr auto MIN_AMOUNT = 0.0001f; 
	if (!env_amount.data || env_amount.data->points.count < 1) {
		return in;
	} 
	const auto amount = blink::search::vec(env_amount, block_positions);
	if (ml::sum(amount) < MIN_AMOUNT) {
		return in;
	} 
	float color = blink::search::one(env_color, block_positions);
	return operator()(in, mode, amount, color, width);
}