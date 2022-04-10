#include "audio.h"
#include "instance.h"
#include "plugin.h"
#include "audio_data.h"
#include <blink/dsp.hpp>

using namespace blink;

namespace gainer {

struct InputValues
{
	struct Envelopes
	{
		ml::DSPVector amp;
		ml::DSPVector pan;

		Envelopes(const AudioData::Envelopes& data, const BlockPositions& positions)
			: amp { data.amp.search_vec(positions) }
			, pan { data.pan.search_vec(positions) }
		{}
	} envelopes;

	struct Sliders
	{
		float amp;
		float pan;

		Sliders(const AudioData::Sliders& data)
			: amp { data.amp.value }
			, pan { data.pan.value }
		{}
	} sliders;

	InputValues(const AudioData& data, const BlockPositions& positions)
		: envelopes { data.envelopes, positions }
		, sliders { data.sliders }
	{
	}
};

Audio::Audio(Instance* instance)
	: blink::EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

void Audio::reset()
{
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	InputValues input_values { { plugin_->params(), unit_state.parameter_data }, block_positions() };

	const ml::DSPVectorArray<2> in_vec(in);

	auto out_vec { stereo_pan(in_vec, input_values.sliders.pan, input_values.envelopes.pan) };

	out_vec *= ml::repeatRows<2>(input_values.envelopes.amp) * input_values.sliders.amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

} // gainer
