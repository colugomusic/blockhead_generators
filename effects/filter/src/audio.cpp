#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(const Filter* plugin)
	: plugin_(plugin)
	, prev_pos_(std::numeric_limits<float>::max())
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	begin_process(buffer);

	ml::DSPVector block_positions;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		block_positions[i] = float(buffer->positions[i] - buffer->data_offset);
	}

	struct Data
	{
		const blink_EnvelopeData* env_freq;
		const blink_EnvelopeData* env_res;
	} data;

	data.env_freq = plugin_->get_envelope_data(buffer->parameter_data, int(Filter::ParameterIndex::Env_Freq));
	data.env_res  = plugin_->get_envelope_data(buffer->parameter_data, int(Filter::ParameterIndex::Env_Res));

	float freq;
	float res;

	plugin_->env_freq().search_vec(data.env_freq, block_positions.getConstBuffer(), 1, prev_pos_, &freq);
	plugin_->env_res().search_vec(data.env_res, block_positions.getConstBuffer(), 1, prev_pos_, &res);

	res = ml::lerp(1.0f, 0.1f, res);

	const auto omega = math::convert::linear_to_filter_hz(freq) / buffer->sample_rate;
	const auto coeffs = ml::Lopass::coeffs(omega, res);

	lopass_[0].mCoeffs = coeffs;
	lopass_[1].mCoeffs = coeffs;

	const ml::DSPVectorArray<2> in_vec(in);

	const auto L = lopass_[0](in_vec.constRow(0));
	const auto R = lopass_[1](in_vec.constRow(1));

	const auto out_vec = ml::concatRows(L, R);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	prev_pos_ = block_positions[kFloatsPerDSPVector - 1];

	return BLINK_OK;
}

blink_Error Audio::reset()
{
	return BLINK_OK;
}
