#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include "simplexnoise1234.h"
#include <blink/dsp.hpp>
#include <blink/math.hpp>
#include <cassert>

using namespace blink;

namespace berk {

struct InputValues
{
	struct Envelopes
	{
		ml::DSPVector amp;
		ml::DSPVector pan;
		ml::DSPVector index;
		ml::DSPVector diameter;
		ml::DSPVector tongue_index;
		ml::DSPVector tongue_diameter;
		ml::DSPVector fricative_intensity;
		float pitch;
		float buzz;
		float quality;

		Envelopes(const AudioData::Envelopes& data, const BlockPositions& positions)
			: amp { data.amp.search_vec(positions) }
			, pan { data.pan.search_vec(positions) }
			, index { data.index.search_vec(positions) }
			, diameter { data.diameter.search_vec(positions) }
			, tongue_index { data.tongue_index.search_vec(positions) }
			, tongue_diameter { data.tongue_diameter.search_vec(positions) }
			, fricative_intensity { data.fricative_intensity.search_vec(positions) }
			, pitch { data.pitch.search(positions) }
			, buzz { data.buzz.search(positions) }
			, quality { data.quality.search(positions) }
		{}
	} envelopes;

	struct Sliders
	{
		float amp;
		float pan;
		float pitch;

		Sliders(const AudioData::Sliders& data)
			: amp { data.amp.value }
			, pan { data.pan.value }
			, pitch { data.pitch.value }
		{}
	} sliders;

	InputValues(const AudioData& data, const BlockPositions& positions)
		: envelopes { data.envelopes, positions }
		, sliders { data.sliders }
	{
	}
};

Audio::Audio(Instance* instance)
	: SynthUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out)
{
	InputValues input_values { { plugin_->params(), unit_state.parameter_data }, block_positions() };
	
	static const ml::DSPVector MIN_INDEX(4.0f);
	static const ml::DSPVector MAX_INDEX(44.0f);

	static const ml::DSPVector MIN_DIAMETER(-1.0f);
	static const ml::DSPVector MAX_DIAMETER(3.0f);

	static const ml::DSPVector MIN_TONGUE_INDEX(12.0f);
	static const ml::DSPVector MAX_TONGUE_INDEX(29.0f);

	static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
	static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);

	const auto min_position = ml::min(ml::intToFloat(block_positions().positions.pos));
	const auto gate = min_position >= -unit_state.data_offset;

	const auto model_SR = int(std::pow(2.0f, input_values.envelopes.quality - 1.0f) * 44100.0f);
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
		glottis_input.buzz = math::convert::bi_to_uni(input_values.envelopes.buzz);
		glottis_input.pitch = input_values.envelopes.pitch + input_values.sliders.pitch;

		const auto glottal_output = glottis_(model_SR, speed, glottis_input);

		Tract::Input tract_input;

		tract_input.fricatives = true;
		tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, input_values.envelopes.diameter);
		tract_input.fricative_intensity = input_values.envelopes.fricative_intensity;
		tract_input.fricative_noise = fricative_noise * glottis_.noise_modulator();
		tract_input.glottal_output = glottal_output;
		tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, math::convert::bi_to_uni(input_values.envelopes.index));
		tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, input_values.envelopes.tongue_diameter);
		tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, math::convert::bi_to_uni(input_values.envelopes.tongue_index));

		return tract_(model_SR, speed, tract_input);
	};

	auto out_vec { stereo_pan(ml::repeatRows<2>(resampler_(source, speed)), input_values.sliders.pan, input_values.envelopes.pan) };

	out_vec *= ml::repeatRows<2>(input_values.envelopes.amp) * input_values.sliders.amp;

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	glottis_.reset();
	tract_.reset();
}

} // berk