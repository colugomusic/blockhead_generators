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
		const blink_EnvelopeData* env_buzz;
		const blink_EnvelopeData* env_index;
		const blink_EnvelopeData* env_diameter;
		const blink_EnvelopeData* env_tongue_index;
		const blink_EnvelopeData* env_tongue_diameter;
		const blink_EnvelopeData* env_fricative_intensity;
		const blink_EnvelopeData* env_quality;
	} data;

	data.env_amp = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Amp));
	data.env_pitch = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Pitch));
	data.env_buzz = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Buzz));
	data.env_index = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Index));
	data.env_diameter = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Diameter));
	data.env_tongue_index = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_TongueIndex));
	data.env_tongue_diameter = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_TongueDiameter));
	data.env_fricative_intensity = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Fricative_Intensity));
	data.env_quality = plugin_->get_envelope_data(buffer->parameter_data, int(Berk::ParameterIndex::Env_Quality));

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions());
	const auto pitch = plugin_->env_pitch().search(data.env_pitch, block_positions().positions[0]);
	const auto buzz = plugin_->env_buzz().search(data.env_buzz, block_positions().positions[0]);
	const auto index = plugin_->env_index().search_vec(data.env_index, block_positions());
	const auto diameter = plugin_->env_diameter().search_vec(data.env_diameter, block_positions());
	const auto tongue_index = plugin_->env_tongue_index().search_vec(data.env_tongue_index, block_positions());
	const auto tongue_diameter = plugin_->env_tongue_diameter().search_vec(data.env_tongue_diameter, block_positions());
	const auto fricative_intensity = plugin_->env_fricative_intensity().search_vec(data.env_fricative_intensity, block_positions());
	const auto quality = plugin_->env_quality().search(data.env_quality, block_positions().positions[0]);

	static const ml::DSPVector MIN_INDEX(4.0f);
	static const ml::DSPVector MAX_INDEX(44.0f);

	static const ml::DSPVector MIN_DIAMETER(-1.0f);
	static const ml::DSPVector MAX_DIAMETER(3.0f);

	static const ml::DSPVector MIN_TONGUE_INDEX(12.0f);
	static const ml::DSPVector MAX_TONGUE_INDEX(29.0f);

	static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
	static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);

	ml::DSPVector out_vec;

	const auto model_SR = int(math::lerp(22050.0f, 88200.0f, quality));

	auto source = [&]()
	{
		const auto noise = noise_();

		aspirate_filter_.mCoeffs = ml::Bandpass::coeffs(500.0f / model_SR, 0.9f);
		fricative_filter_.mCoeffs = ml::Bandpass::coeffs(1000.0f / model_SR, 0.9f);

		const auto fricative_noise = fricative_filter_(noise);
		const auto aspirate_noise = aspirate_filter_(noise);

		const auto glottal_output = glottis_(model_SR, pitch, buzz, aspirate_noise);

		Tract::Input tract_input;

		tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, diameter);
		tract_input.fricative_intensity = fricative_intensity;
		tract_input.fricative_noise = fricative_noise * glottis_.noise_modulator();
		tract_input.glottal_output = glottal_output;
		tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, index);
		tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, tongue_diameter);
		tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, tongue_index);

		return tract_(model_SR, tract_input);
	};

	out_vec = resampler_(source, float(model_SR) / buffer->sample_rate) * amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(0), out + kFloatsPerDSPVector);

	return BLINK_OK;
}
void Audio::reset()
{
	glottis_.reset();
	tract_.reset();
}
