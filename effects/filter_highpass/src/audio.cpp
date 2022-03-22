#include "audio.h"
#include "audio_data.h"
#include "instance.h"
#include "plugin.h"

using namespace blink;

namespace highpass {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	const auto freq = data.envelopes.freq.search(block_positions());
	auto res = data.envelopes.res.search(block_positions());

	res = ml::lerp(1.0f, 0.1f, res);

	const auto omega = math::convert::linear_to_filter_hz(freq) / SR();
	const auto coeffs = ml::Hipass::coeffs(omega, res);

	filter_[0].mCoeffs = coeffs;
	filter_[1].mCoeffs = coeffs;

	const ml::DSPVectorArray<2> in_vec(in);

	const auto L = filter_[0](in_vec.constRow(0));
	const auto R = filter_[1](in_vec.constRow(1));

	auto out_vec = ml::concatRows(L, R);

	const auto mix = data.envelopes.mix.search_vec(block_positions());

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	filter_[0] = {};
	filter_[1] = {};
}

} // highpass