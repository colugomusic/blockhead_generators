#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace saturator {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	const auto drive = data.envelopes.drive.search_vec(block_positions());
	const auto amp = data.envelopes.amp.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	ml::DSPVectorArray<2> in_vec(in);

	auto out_vec = saturator_(in_vec, ml::repeatRows<2>(drive)) * ml::repeatRows<2>(amp);

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
}

} // saturator
