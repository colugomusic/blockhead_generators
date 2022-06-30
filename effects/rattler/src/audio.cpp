#include "audio.h"
#include "instance.h"
#include "plugin.h"
#include "audio_data.h"
#include <blink/dsp.hpp>

using namespace blink;

namespace rattler {

struct InputValues
{
	struct Envelopes
	{
		ml::DSPVector time;
		ml::DSPVector feedback;
		ml::DSPVector width;
		ml::DSPVector dry;
		ml::DSPVector wet;

		Envelopes(const AudioData::Envelopes& data, const BlockPositions& positions)
			: time { data.time.search_vec(positions) }
			, feedback { data.feedback.search_vec(positions) }
			, width { data.width.search_vec(positions) }
			, dry { data.dry.search_vec(positions) }
			, wet { data.wet.search_vec(positions) }
		{}
	} envelopes;

	struct Sliders
	{
		float dry;
		float wet;
		float width;

		Sliders(const AudioData::Sliders& data)
			: dry { data.dry.value }
			, wet { data.wet.value }
			, width { data.width.value }
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

auto Audio::stream_init() -> void
{
	delays_[0].setMaxDelayInSamples(2.0f * SR());
	delays_[1].setMaxDelayInSamples(2.0f * SR());
	fade_in_.setGlideTimeInSamples(0.1f * SR());
}

auto Audio::reset() -> void
{
	feedback_ = {};
	delays_[0].clear();
	delays_[1].clear();
	fade_in_.setValue(0.0f);
}

auto Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) -> blink_Error
{
	InputValues input_values{ { plugin_->params(), unit_state.parameter_data }, block_positions() };

	const auto dry { input_values.envelopes.dry + input_values.sliders.dry };
	const auto wet { input_values.envelopes.wet + input_values.sliders.wet };
	const auto feedback{ input_values.envelopes.feedback };
	auto width{ input_values.envelopes.width + input_values.sliders.width };

	width = width * ml::abs(width);

	auto time_ms{ ml::repeatRows<2>(math::convert::linear_to_delay_time_ms(input_values.envelopes.time)) };

	time_ms.row(0) += width;
	time_ms.row(1) -= width;

	const auto time_samples{ time_ms / 1000 * float(SR()) };

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec{};

	auto delay_input{ (in_vec * ml::repeatRows<2>(wet) * ml::repeatRows<2>(fade_in_(1.0f))) + feedback_ };

	for (auto channel{ 0 }; channel < 2; channel++)
	{
		out_vec.row(channel) = delays_[channel](delay_input.constRow(channel), time_samples.constRow(channel));
	}

	feedback_ = out_vec * ml::repeatRows<2>(feedback);

	out_vec += ml::repeatRows<2>(dry) * in_vec;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

} // rattler
