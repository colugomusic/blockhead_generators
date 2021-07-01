#include "audio.h"
#include "plugin.h"
#include "simplexnoise1234.h"

using namespace blink;

Audio::Audio(Voice* plugin, int instance_group)
	: Synth(plugin, instance_group)
	, plugin_(plugin)
{
}

blink_Error Audio::process(const blink_SynthBuffer* buffer, float* out)
{
	struct Data
	{
		const blink_EnvelopeData* env_amp;
	} data;

	data.env_amp = plugin_->get_envelope_data(buffer->parameter_data, int(Voice::ParameterIndex::Env_Amp));

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions());

	const auto noise = noise_();

	aspirate_filter_.mCoeffs  = ml::Bandpass::coeffs(500.0f / float(buffer->sample_rate), 0.5f);
	fricative_filter_.mCoeffs = ml::Bandpass::coeffs(1000.0f / float(buffer->sample_rate), 0.5f);

	ml::DSPVector out_vec;

	out_vec += voice(buffer->sample_rate, aspirate_filter_(noise), fricative_filter_(noise));

	out_vec *= amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(0), out + kFloatsPerDSPVector);

	return BLINK_OK;
}
ml::DSPVector Audio::voice(int SR, const ml::DSPVector& aspirate_noise, const ml::DSPVector& fricative_noise)
{
	ml::DSPVector out;

	const auto glottal_output = glottis_(SR, aspirate_noise);
	const auto vocal_output = tract_(SR, glottal_output, fricative_noise);

	return vocal_output;
}

void Audio::reset()
{
}
