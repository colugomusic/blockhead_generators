#include "audio.h"
#include "instance.h"
#include "plugin.h"
#include "audio_data.h"

using namespace blink;

namespace freeze {

Audio::Audio(Instance* instance)
	: blink::EffectUnit{ instance }
	, plugin_{ instance->get_plugin() }
	, instance_{ instance }
	, controller_{ plugin_, instance->buffer }
	, particle_{ controller_, [this](int vector_index, std::size_t row, float pos) { return buffer_read(vector_index, row, pos); } }
{
}

void Audio::reset()
{
	instance_->buffer.clear();
	particle_.queue_reset();
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out)
{
	if (!instance_->master_unit)
	{
		instance_->master_unit = this;
		record_ = false;
	}

	AudioData data(*plugin_, unit_state.parameter_data);

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec(in);

	controller_.process(data, buffer, SR(), block_positions());

	ml::DSPVector dry;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		if (instance_->master_unit == this)
		{
			if (!record_)
			{
				const auto local_block_position = block_positions().positions[i] + std::int32_t(unit_state.data_offset);

				if (local_block_position >= 0)
				{
					record_ = true;
					particle_.queue_reset();
				}
			}

			if (record_)
			{
				instance_->buffer.process(in_vec.constRow(0)[i], in_vec.constRow(1)[i]);
			}
		}
		
		auto LR = particle_.process(i, &dry[i]);

		out_vec.row(0)[i] = LR.L;
		out_vec.row(1)[i] = LR.R;
	}

	out_vec += ml::repeatRows<2>(dry) * in_vec;

	const auto mix = data.envelopes.mix.search_vec(block_positions());

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

float Audio::buffer_read(int vector_index, std::size_t row, float pos) const
{
	int next = int(std::ceil(pos));
	int prev = int(std::floor(pos));
	float x = pos - float(prev);

	const auto next_value = instance_->buffer.at(row, next);
	const auto prev_value = instance_->buffer.at(row, prev);

	return (x * (next_value - prev_value)) + prev_value;
}

} // freeze