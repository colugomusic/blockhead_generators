#include "audio.h"
#include "plugin.h"
#include "simplexnoise1234.h"
#include <blink/math.hpp>
#include <cassert>

using namespace blink;

Audio::Audio(Berk* plugin, int instance_group)
	: Synth(plugin, instance_group)
	, plugin_(plugin)
{
	buffer_.resize(kFloatsPerDSPVector * 2);
}

blink_Error Audio::process(const blink_SynthBuffer* buffer, float* out)
{
	struct Data
	{
		const blink_EnvelopeData* env_amp;
		const blink_EnvelopeData* env_pitch;
		const blink_EnvelopeData* env_formant;
		const blink_EnvelopeData* env_index;
		const blink_EnvelopeData* env_diameter;
		const blink_EnvelopeData* env_tongue_index;
		const blink_EnvelopeData* env_tongue_diameter;
		const blink_EnvelopeData* env_fricative_intensity;
	} data;

	data.env_amp = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Amp));
	data.env_pitch = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Pitch));
	data.env_formant = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Formant));
	data.env_index = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Index));
	data.env_diameter = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Diameter));
	data.env_tongue_index = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_TongueIndex));
	data.env_tongue_diameter = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_TongueDiameter));
	data.env_fricative_intensity = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Fricative_Intensity));

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions());
	const auto pitch = plugin_->env_pitch().search(data.env_pitch, block_positions().positions[0]);
	const auto formant = plugin_->env_formant().search(data.env_formant, block_positions().positions[0]);
	const auto index = plugin_->env_index().search_vec(data.env_index, block_positions());
	const auto diameter = plugin_->env_diameter().search_vec(data.env_diameter, block_positions());
	const auto tongue_index = plugin_->env_tongue_index().search_vec(data.env_tongue_index, block_positions());
	const auto tongue_diameter = plugin_->env_tongue_diameter().search_vec(data.env_tongue_diameter, block_positions());
	const auto fricative_intensity = plugin_->env_fricative_intensity().search_vec(data.env_fricative_intensity, block_positions());

	static const ml::DSPVector MIN_INDEX(4.0f);
	static const ml::DSPVector MAX_INDEX(44.0f);

	static const ml::DSPVector MIN_DIAMETER(-1.0f);
	static const ml::DSPVector MAX_DIAMETER(3.0f);

	static const ml::DSPVector MIN_TONGUE_INDEX(12.0f);
	static const ml::DSPVector MAX_TONGUE_INDEX(29.0f);

	static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
	static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);

	ml::DSPVector out_vec;
	
	static constexpr auto MODEL_SR = 44100;

	auto source = [&]()
	{
		const auto noise = noise_();

		aspirate_filter_.mCoeffs = ml::Bandpass::coeffs(500.0f / float(buffer->sample_rate), 0.5f);
		fricative_filter_.mCoeffs = ml::Bandpass::coeffs(1000.0f / float(buffer->sample_rate), 0.5f);

		const auto fricative_noise = fricative_filter_(noise);
		const auto aspirate_noise = aspirate_filter_(noise);

		const auto glottal_output = glottis_(MODEL_SR, pitch, formant, aspirate_noise);

		Tract::Input tract_input;

		tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, diameter);
		tract_input.fricative_intensity = fricative_intensity;
		tract_input.fricative_noise = fricative_noise * glottis_.noise_modulator();
		tract_input.glottal_output = glottal_output;
		tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, index);
		tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, tongue_diameter);
		tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, tongue_index);

		return tract_(MODEL_SR, tract_input);
	};

	out_vec = resampler_(source, float(MODEL_SR) / buffer->sample_rate) * amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(0), out + kFloatsPerDSPVector);

	return BLINK_OK;
}
void Audio::reset()
{
	glottis_.reset();
	tract_.reset();
}
