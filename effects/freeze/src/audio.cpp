#include "audio.h"
#include "plugin.h"
#include "audio_data.h"

using namespace blink;

Audio::Audio(const Freeze* plugin)
	: plugin_(plugin)
	, controller_(plugin, freeze_buffer_, block_traverser_)
	, particle_(controller_, [this](int vector_index, std::size_t row, float pos) { return buffer_read(vector_index, row, pos); })
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	begin_process(buffer);

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
		freeze_buffer_.process(block_traverser_.get_resets()[i] > 0, in_vec.constRow(0)[i], in_vec.constRow(1)[i]);
		
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

blink_Error Audio::reset()
{
	block_traverser_.set_reset(0);
	freeze_buffer_.clear();
	particle_.clear();

	return BLINK_OK;
}

float Audio::buffer_read(int vector_index, std::size_t row, float pos) const
{
	int next = int(std::ceil(pos));
	int prev = int(std::floor(pos));
	float x = pos - float(prev);

	const auto next_value = freeze_buffer_.at(row, next);
	const auto prev_value = freeze_buffer_.at(row, prev);

	return (x * (next_value - prev_value)) + prev_value;
}
