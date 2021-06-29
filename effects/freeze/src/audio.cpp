#include "audio.h"
#include "plugin.h"
#include "audio_data.h"

using namespace blink;

Audio::Audio(Freeze* plugin, int instance_group, std::shared_ptr<Freeze::InstanceGroupData> instance_group_data)
	: blink::Effect(plugin, instance_group)
	, plugin_(plugin)
	, instance_group_data_(instance_group_data)
	, controller_(plugin, instance_group_data->buffer, block_traverser_)
	, particle_(controller_, [this](int vector_index, std::size_t row, float pos) { return buffer_read(vector_index, row, pos); })
{
}

void Audio::reset()
{
	particle_.queue_reset();
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	if (!instance_group_data_->master_instance)
	{
		instance_group_data_->master_instance = this;
		record_ = false;
	}

	block_traverser_.generate(block_positions());

	AudioData data;

	data.env_pitch = plugin_->get_envelope_data(buffer->parameter_data, int(Freeze::ParameterIndex::Env_Pitch));
	data.env_formant = plugin_->get_envelope_data(buffer->parameter_data, int(Freeze::ParameterIndex::Env_Formant));
	data.env_mix = plugin_->get_envelope_data(buffer->parameter_data, int(Freeze::ParameterIndex::Env_Mix));
	data.slider_pitch = plugin_->get_slider_data(buffer->parameter_data, int(Freeze::ParameterIndex::Sld_Pitch));

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec(in);

	controller_.process(data, *buffer);

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		if (instance_group_data_->master_instance == this)
		{
			const auto local_block_position = block_positions().positions[i] + std::int32_t(block_positions().data_offset);

			if (local_block_position >= 0)
			{
				record_ = true;
			}

			if (record_)
			{
				instance_group_data_->buffer.process(in_vec.constRow(0)[i], in_vec.constRow(1)[i]);
			}
		}
		
		auto LR = particle_.process(i);

		out_vec.row(0)[i] = LR.L;
		out_vec.row(1)[i] = LR.R;
	}

	const auto mix = plugin_->env_mix().search_vec(data.env_mix, block_positions());

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

	const auto next_value = instance_group_data_->buffer.at(row, next);
	const auto prev_value = instance_group_data_->buffer.at(row, prev);

	return (x * (next_value - prev_value)) + prev_value;
}
