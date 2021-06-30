#include "audio.h"
#include "plugin.h"
#include "simplexnoise1234.h"

using namespace blink;

Audio::Audio(Voice* plugin, int instance_group)
	: Synth(plugin, instance_group)
	, plugin_(plugin)
{
	const auto t = 0.5f;

	glottis_.ui_tenseness = 1.0f - std::cos(t * float(M_PI) * 0.5f);
	glottis_.loudness = std::pow(glottis_.ui_tenseness, 0.25f);

	glottis_setup_waveform(0.0f);
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

void Audio::glottis_setup_waveform(float lambda)
{
	glottis_.frequency = glottis_.old_frequency * (1.0f - lambda) + glottis_.new_frequency * lambda;

	const auto tenseness = glottis_.old_tenseness * (1.0f - lambda) + glottis_.new_tenseness * lambda;

	glottis_.rd = 3.0f * (1.0f - tenseness);
	glottis_.waveform_length = 1.0f / glottis_.frequency;

	const auto rd = std::clamp(glottis_.rd, 0.5f, 2.7f);

	const auto ra = -0.01f + 0.048f * rd;
	const auto rk = 0.224f + 0.118f * rd;
	const auto rg = (rk / 4.0f) * (0.5f + 1.2f * rk) / (0.11f * rd - ra * (0.5f + 1.2f * rk));

	const auto ta = ra;
	const auto tp = 1.0f / (2.0f * rg);
	const auto te = tp + tp * rk;

	const auto epsilon = 1.0f / ta;
	const auto shift = std::exp(-epsilon * (1.0f - te));
	const auto delta = 1.0f - shift;

	auto rhs_integral = (1.0f / epsilon) * (shift - 1.0f) + (1.0f - te) * shift;

	rhs_integral /= delta;

	const auto total_lower_integral = - (te - tp) / 2.0f + rhs_integral;
	const auto total_upper_integral = -total_lower_integral;

	const auto omega = float(M_PI) / tp;
	const auto s = std::sin(omega * te);
	const auto y = -float(M_PI) * s * total_upper_integral / (tp * 2.0f);
	const auto z = std::log(y);
	const auto alpha = z / (tp / 2.0f - te);
	const auto e0 = -1.0f / (s * std::exp(alpha * te));
	
	glottis_.alpha = alpha;
	glottis_.e0 = e0;
	glottis_.epsilon = epsilon;
	glottis_.shift = shift;
	glottis_.delta = delta;
	glottis_.te = te;
	glottis_.omega = omega;
}

float Audio::glottis_normalized_lf_waveform(float t)
{
	float out;

	if (t > glottis_.te)
	{
		out = (-std::exp(-glottis_.epsilon * (t - glottis_.te)) + glottis_.shift) / glottis_.delta;
	}
	else
	{
		out = glottis_.e0 * std::exp(glottis_.alpha * t) * std::sin(glottis_.omega * t);
	}

	return out * glottis_.intensity * glottis_.loudness;
}

float Audio::glottis_noise_modulator()
{
	const auto voiced = 0.1f + 0.2f * std::max(0.0f, std::sin(float(M_PI) * 2.0f * glottis_.time_in_waveform / glottis_.waveform_length));

	return glottis_.ui_tenseness * glottis_.intensity * voiced + (1.0f - glottis_.ui_tenseness * glottis_.intensity) * 0.3f;
}

float Audio::glottis_step(int SR, float lambda, float noise)
{
	const auto time_step = 1.0f / float(SR);

	glottis_.time_in_waveform += time_step;
	glottis_.total_time += time_step;

	if (glottis_.time_in_waveform > glottis_.waveform_length)
	{
		glottis_.time_in_waveform -= glottis_.waveform_length;
		glottis_setup_waveform(lambda);
	}

	float out = glottis_normalized_lf_waveform(glottis_.time_in_waveform / glottis_.waveform_length);
	auto aspiration  = glottis_.intensity * (1.0f - std::sqrt(glottis_.ui_tenseness)) * glottis_noise_modulator() * noise;

	aspiration *= 0.2f + 0.02f * snoise1(glottis_.total_time * 1.99f);

	out += aspiration;

	return out;
}

void Audio::glottis_finish_block()
{
	auto vibrato = 0.0f;

	//vibrato += glottis_.vibrato_amount * std::sin(2.0f * float(M_PI) * glottis_.total_time * glottis_.vibrato_frequency);
	//vibrato += 0.02f * snoise1(glottis_.total_time * 4.07f);
	//vibrato += 0.04f * snoise1(glottis_.total_time * 2.15f);

	if (glottis_.ui_frequency > glottis_.smooth_frequency)
	{
		glottis_.smooth_frequency = std::min(glottis_.smooth_frequency * 1.1f, glottis_.ui_frequency);
	}

	if (glottis_.ui_frequency < glottis_.smooth_frequency)
	{
		glottis_.smooth_frequency = std::max(glottis_.smooth_frequency / 1.1f, glottis_.ui_frequency);
	}

	glottis_.old_frequency = glottis_.new_frequency;
	glottis_.new_frequency = glottis_.smooth_frequency * (1.0f + vibrato);
	glottis_.old_tenseness = glottis_.new_tenseness;
	glottis_.new_tenseness = glottis_.ui_tenseness + 0.1f * snoise1(glottis_.total_time * 0.46f) + 0.05f * snoise1(glottis_.total_time * 0.36f);

	// if (!is_touched && always_voice)
	// {
		glottis_.new_tenseness += (3.0f - glottis_.ui_tenseness) * (1.0f - glottis_.intensity);
	// }

	// if (is_touched || always_voice)
	// {
		glottis_.intensity += 0.13f;
	// }
	// else
	// {
	//	glottis_.intensity -= 0.05f;
	// }

	glottis_.intensity = std::clamp(glottis_.intensity, 0.0f, 1.0f);
}

ml::DSPVector Audio::voice(int SR, const ml::DSPVector& aspirate_noise, const ml::DSPVector& fricative_noise)
{
	ml::DSPVector out;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		const auto lambda1 = float(i) / kFloatsPerDSPVector;
		const auto lambda2 = (float(i) + 0.5f) / kFloatsPerDSPVector;
		const auto glottal_output = glottis_step(SR, lambda1, aspirate_noise[i]);

		out[i] = glottal_output;
	}

	glottis_finish_block();

	return out;
	/*
	var inputArray1 = event.inputBuffer.getChannelData(0);
	var inputArray2 = event.inputBuffer.getChannelData(1);
	var outArray = event.outputBuffer.getChannelData(0);
	for (var j = 0, N = outArray.length; j < N; j++)
	{
		var lambda1 = j / N;
		var lambda2 = (j + 0.5) / N;
		var glottalOutput = Glottis.runStep(lambda1, inputArray1[j]);

		var vocalOutput = 0;
		//Tract runs at twice the sample rate 
		Tract.runStep(glottalOutput, inputArray2[j], lambda1);
		vocalOutput += Tract.lipOutput + Tract.noseOutput;
		Tract.runStep(glottalOutput, inputArray2[j], lambda2);
		vocalOutput += Tract.lipOutput + Tract.noseOutput;
		outArray[j] = vocalOutput * 0.125;
	}
	Glottis.finishBlock();
	Tract.finishBlock();
	*/
}

void Audio::reset()
{
}
