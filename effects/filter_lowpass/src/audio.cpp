#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(const Filter* plugin)
	: plugin_(plugin)
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	begin_process(buffer);

	struct Data
	{
		const blink_EnvelopeData* env_freq;
		const blink_EnvelopeData* env_res;
		const blink_EnvelopeData* env_mix;
	} data;

	data.env_freq = plugin_->get_envelope_data(buffer->parameter_data, int(Filter::ParameterIndex::Env_Freq));
	data.env_res  = plugin_->get_envelope_data(buffer->parameter_data, int(Filter::ParameterIndex::Env_Res));
	data.env_mix = plugin_->get_envelope_data(buffer->parameter_data, int(Filter::ParameterIndex::Env_Mix));

	float freq;
	float res;

	plugin_->env_freq().search_vec(data.env_freq, block_positions(), 1, &freq);
	plugin_->env_res().search_vec(data.env_res, block_positions(), 1, &res);

	res = ml::lerp(1.0f, 0.1f, res);

	const auto omega = math::convert::linear_to_filter_hz(freq) / buffer->sample_rate;
	const auto coeffs = ml::Lopass::coeffs(omega, res);

	lopass_[0].mCoeffs = coeffs;
	lopass_[1].mCoeffs = coeffs;

	const ml::DSPVectorArray<2> in_vec(in);

	const auto L = lopass_[0](in_vec.constRow(0));
	const auto R = lopass_[1](in_vec.constRow(1));

	auto out_vec = ml::concatRows(L, R);

	const auto mix = plugin_->env_mix().search_vec(data.env_mix, block_positions());

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

blink_Error Audio::reset()
{
	return BLINK_OK;
}