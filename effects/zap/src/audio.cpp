#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace zap {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	auto base_freq = data.envelopes.frequency.search_vec(block_positions());
	const auto res = data.envelopes.resonance.search_vec(block_positions());
	auto spread = data.envelopes.spread.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	const auto transform_spread = [](ml::DSPVector x)
	{
		return x * x * ml::signBit(x);
	};

	base_freq = math::convert::linear_to_filter_hz(base_freq);
	spread = transform_spread(spread);

	const auto offset = spread * 1000.0f;

	static const ml::DSPVector MIN_FREQ(130.0f);
	static const ml::DSPVector MAX_FREQ(4000.0f);

	const auto freq_L = ml::clamp(base_freq - offset, MIN_FREQ, MAX_FREQ);
	const auto freq_R = ml::clamp(base_freq + offset, MIN_FREQ, MAX_FREQ);
	const auto freq = ml::concatRows(freq_L, freq_R);

	ml::DSPVectorArray<2> in_vec(in);

	auto out_vec = filter_(in_vec, SR(), freq, ml::repeatRows<2>(res));

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	filter_.clear();
}

} // zap
