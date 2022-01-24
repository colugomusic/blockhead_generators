#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"
#include <tweak/convert.hpp>

using namespace blink;

namespace compressor {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
	reset();
}

template <size_t ROWS>
static ml::DSPVectorArray<ROWS> linear_to_db(ml::DSPVectorArray<ROWS> x)
{
	return ml::map([](float v) { return tweak::convert::linear_to_db(v); }, x);
}

template <size_t ROWS>
static ml::DSPVectorArray<ROWS> linear_to_ratio(ml::DSPVectorArray<ROWS> x)
{
	return ml::map([](float v) { return tweak::convert::linear_to_ratio(v); }, x);
}

template <size_t ROWS>
static ml::DSPVectorArray<ROWS> db_to_linear(ml::DSPVectorArray<ROWS> x)
{
	return ml::map([](float v) { return tweak::convert::db_to_linear(v); }, x);
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	const auto attack = data.envelopes.attack.search(block_positions());
	const auto release = data.envelopes.release.search(block_positions());
	auto threshold = data.envelopes.threshold.search_vec(block_positions());
	auto ratio = data.envelopes.ratio.search_vec(block_positions());
	const auto knee = data.envelopes.knee.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	threshold = linear_to_db(threshold);
	ratio = linear_to_ratio(ratio);

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	static const auto EPSILON = ml::DSPVector(0.0000001f);
	static const auto ONE = ml::DSPVector(1.0f);

	ml::DSPVectorArray<2> reduction;

	if (!data.options.stereo.value) // mono
	{
		mono_.env_follower_0.set_SR(SR());
		mono_.env_follower_0.set_release(release);
		mono_.env_follower_1.set_SR(SR());
		mono_.env_follower_1.set_time(attack);

		const auto peak = mono_.env_follower_1(mono_.env_follower_0((in_vec.constRow(0) + in_vec.constRow(1)) * 0.5f));
		const auto peak_db = linear_to_db(ml::clamp(ml::abs(peak), EPSILON, ONE));

		const auto pre_knee = peak_db - threshold;
		const auto post_knee = (ml::sqrt((pre_knee * pre_knee) + (knee * knee)) + pre_knee) * 0.5f;

		const auto r = (1.0f - (1.0f / ratio));
		const auto db_reduction = r * post_knee;
		const auto makeup_db = r * ((0.0f - threshold) / 2.0f);

		reduction = ml::repeatRows<2>(db_to_linear(makeup_db - db_reduction));
	}
	else // stereo
	{
		stereo_.env_follower_0.set_SR(SR());
		stereo_.env_follower_0.set_release(release);
		stereo_.env_follower_1.set_SR(SR());
		stereo_.env_follower_1.set_time(attack);

		const auto peak = stereo_.env_follower_1(stereo_.env_follower_0(in_vec));
		const auto peak_db = linear_to_db(ml::clamp(ml::abs(peak), ml::repeatRows<2>(EPSILON), ml::repeatRows<2>(ONE)));

		const auto knee2 = ml::repeatRows<2>(knee);
		const auto threshold2 = ml::repeatRows<2>(threshold);
		const auto pre_knee = peak_db - threshold2;
		const auto post_knee = (ml::sqrt((pre_knee * pre_knee) + (knee2 * knee2)) + pre_knee) * 0.5f;

		const auto r = (1.0f - (1.0f / ml::repeatRows<2>(ratio)));
		const auto db_reduction = r * post_knee;
		const auto makeup_db = r * ((0.0f - threshold2) / 2.0f);

		reduction = db_to_linear(makeup_db - db_reduction);
	}

	out_vec = in_vec * reduction;
 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	mono_.env_follower_0.reset();
	mono_.env_follower_1.reset();
	stereo_.env_follower_0.reset();
	stereo_.env_follower_1.reset();
}

} // compressor
