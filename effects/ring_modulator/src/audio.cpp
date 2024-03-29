#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace ring_modulator {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	const auto pitch = data.envelopes.pitch.search_vec(block_positions()) + 60.0f;
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	const auto omega = math::convert::pitch_to_frequency(pitch) / float(SR());
	const auto sine = sine_(omega);
	const auto am = (sine + 1.0f) / 2.0f;
	const auto out_vec = ml::DSPVectorArray<2>(in) * ml::repeatRows<2>(ml::lerp(ml::DSPVector(1.0f), am, mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	sine_.clear();
}

} // ring_modulator