#include "audio.h"
#include "instance.h"
#include "plugin.h"

using namespace blink;

namespace highpass {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	struct Data
	{
		const blink_EnvelopeData* env_freq;
		const blink_EnvelopeData* env_res;
		const blink_EnvelopeData* env_mix;
	} data;

	data.env_freq = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Freq));
	data.env_res  = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Res));
	data.env_mix = plugin_->get_envelope_data(buffer->parameter_data, int(Plugin::ParameterIndex::Env_Mix));

	float freq;
	float res;

	plugin_->env_freq().search_vec(data.env_freq, block_positions(), 1, &freq);
	plugin_->env_res().search_vec(data.env_res, block_positions(), 1, &res);

	res = ml::lerp(1.0f, 0.1f, res);

	const auto omega = math::convert::linear_to_filter_hz(freq) / SR();
	const auto coeffs = ml::Hipass::coeffs(omega, res);

	filter_[0].mCoeffs = coeffs;
	filter_[1].mCoeffs = coeffs;

	const ml::DSPVectorArray<2> in_vec(in);

	const auto L = filter_[0](in_vec.constRow(0));
	const auto R = filter_[1](in_vec.constRow(1));

	auto out_vec = ml::concatRows(L, R);

	const auto mix = plugin_->env_mix().search_vec(data.env_mix, block_positions());

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
}

} // highpass