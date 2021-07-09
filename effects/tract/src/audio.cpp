#include "audio.h"
#include "audio_data.h"
#include "plugin.h"

using namespace blink;

namespace tract {

static constexpr auto BASE_MODEL_SR = 44100;
static constexpr auto ROWS = 2;

Audio::Audio(Tract* plugin, int instance_group)
	: Effect(plugin, instance_group)
	, plugin_(plugin)
{
}

void Audio::stream_init()
{
	const auto buffer_size = 2 * int(std::ceil(kFloatsPerDSPVector * ROWS * (float(SR()) / float(BASE_MODEL_SR / 2))));

	input_buffer_.resize(buffer_size);
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	const auto index = data.envelopes.index.search_vec(block_positions());
	const auto diameter = data.envelopes.diameter.search_vec(block_positions());
	const auto tongue_index = data.envelopes.tongue_index.search_vec(block_positions());
	const auto tongue_diameter = data.envelopes.tongue_diameter.search_vec(block_positions());
	const auto fricative_intensity = data.envelopes.fricative_intensity.search_vec(block_positions());
	const auto quality = data.envelopes.quality.search(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

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

	ml::DSPVectorArray<2> in_vec(in);

	input_buffer_.write(in_vec);

	const auto model_SR = int(std::pow(2.0f, quality - 1.0f) * float(BASE_MODEL_SR));
	const auto speed = float(model_SR) / SR();
	const auto required_frames = (1.0f / speed) * kFloatsPerDSPVector;

	ml::DSPVectorArray<2> out_vec;

	if (input_buffer_.getReadAvailable() >= required_frames * 2)
	{
		auto source = [&]()
		{
			const auto noise = noise_();

			aspirate_filter_.mCoeffs = ml::Bandpass::coeffs(500.0f / model_SR, 0.9f);
			fricative_filter_.mCoeffs = ml::Bandpass::coeffs(1000.0f / model_SR, 0.9f);

			const auto fricative_noise = fricative_filter_(noise);
			const auto aspirate_noise = aspirate_filter_(noise);

			auto input_source = [&]()
			{
				ml::DSPVectorArray<2> out;

				input_buffer_.read(out);

				return out;
			};

			ml::DSPVectorArray<2> tract_in = input_resampler_(input_source, 1.0f / speed);
			ml::DSPVectorArray<2> tract_out;

			for (int r = 0; r < 2; r++)
			{
				::Tract::Input tract_input;

				tract_input.diameter = ml::lerp(MIN_DIAMETER, MAX_DIAMETER, diameter);
				tract_input.fricative_intensity = fricative_intensity;
				tract_input.fricative_noise = fricative_noise;
				tract_input.glottal_output = tract_in.constRow(r);
				tract_input.index = ml::lerp(MIN_INDEX, MAX_INDEX, index);
				tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, tongue_diameter);
				tract_input.tongue.index = ml::lerp(MIN_TONGUE_INDEX, MAX_TONGUE_INDEX, tongue_index);

				tract_out.row(r) = tract_[r](model_SR, speed, tract_input);
			}

			return tract_out;
		};

		out_vec = resampler_(source, speed);
	}

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	for (int r = 0; r < 2; r++)
	{
		tract_[r].reset();
	}
}

} // tract