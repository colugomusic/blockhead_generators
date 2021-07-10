#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include "simplexnoise1234.h"
#include <blink/math.hpp>
#include <cassert>

using namespace blink;

namespace berk {

Audio::Audio(Instance* instance)
	: SynthUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_SynthBuffer* buffer, float* out)
{
	berk::AudioData data(plugin_, buffer);
	
	const auto amp = data.envelopes.amp.search(block_positions());
	const auto pitch = data.envelopes.pitch.search(block_positions());
	const auto buzz = data.envelopes.buzz.search(block_positions());
	const auto index = data.envelopes.index.search_vec(block_positions());
	const auto diameter = data.envelopes.diameter.search_vec(block_positions());
	const auto tongue_index = data.envelopes.tongue_index.search_vec(block_positions());
	const auto tongue_diameter = data.envelopes.tongue_diameter.search_vec(block_positions());
	const auto fricative_intensity = data.envelopes.fricative_intensity.search_vec(block_positions());
	const auto quality = data.envelopes.quality.search(block_positions());

	static const ml::DSPVector MIN_INDEX(4.0f);
	static const ml::DSPVector MAX_INDEX(44.0f);

	static const ml::DSPVector MIN_DIAMETER(-1.0f);
	static const ml::DSPVector MAX_DIAMETER(3.0f);

	static const ml::DSPVector MIN_TONGUE_INDEX(12.0f);
	static const ml::DSPVector MAX_TONGUE_INDEX(29.0f);

	static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
	static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);

	const auto min_position = ml::min(ml::intToFloat(block_positions().positions.pos));
	const auto gate = min_position >= 0.0f;

	const auto model_SR = int(std::pow(2.0f, quality - 1.0f) * 44100.0f);
	const auto speed = float(model_SR) / SR();

	auto source = [&]()
	{
		if (!gate) return ml::DSPVector();

		const auto noise = noise_();

		aspirate_filter_.mCoeffs = ml::Bandpass::coeffs(500.0f / model_SR, 0.9f);
		fricative_filter_.mCoeffs = ml::Bandpass::coeffs(1000.0f / model_SR, 0.9f);

		const auto fricative_noise = fricative_filter_(noise);
		const auto aspirate_noise = aspirate_filter_(noise);

		Glottis::Input glottis_input;

		glottis_input.aspirate_noise = aspirate_noise;
		glottis_input.buzz = buzz;
		glottis_input.pitch = pitch;

		const auto glottal_output = glottis_(model_SR, speed, glottis_input);

		Tract::Input tract_input;

		tract_input.fricatives = true;
		tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, diameter);
		tract_input.fricative_intensity = fricative_intensity;
		tract_input.fricative_noise = fricative_noise * glottis_.noise_modulator();
		tract_input.glottal_output = glottal_output;
		tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, index);
		tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, tongue_diameter);
		tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, tongue_index);

		return tract_(model_SR, speed, tract_input);
	};

	auto out_vec = resampler_(source, speed) * amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(0), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	glottis_.reset();
	tract_.reset();
}

} // berk