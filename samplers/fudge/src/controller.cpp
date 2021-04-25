#include "controller.h"
#include "audio_data.h"
#include "plugin.h"
#include "convert.h"
#include <blink/bits.hpp>

Controller::Controller(const Fudge* plugin)
	: plugin_(plugin)
{

}

void Controller::process(
	const AudioData& data,
	const blink_SamplerBuffer& buffer,
	const SampleAnalysis* analysis_data,
	const blink::Traverser& block_traverser,
	const blink::BlockPositions& block_positions)
{
	block_positions_ = &block_positions;

	position_traverser_.get_positions(
		data.slider_speed->value,
		data.env_speed,
		data.warp_points,
		block_traverser,
		data.slider_sample_offset->value,
		kFloatsPerDSPVector,
		nullptr,
		&sample_positions_,
		nullptr);

	sample_positions_ /= float(buffer.song_rate) / buffer.sample_info->SR;

	sample_data_ = blink::SampleData(buffer.sample_info, buffer.channel_mode);

	if (data.toggle_reverse->value)
	{
		sample_positions_ = float(buffer.sample_info->num_frames - 1) - sample_positions_;
	}

	if (data.toggle_loop->value)
	{
		sample_positions_ = blink::math::wrap(sample_positions_, float(buffer.sample_info->num_frames));
	}

	buffer_ = &buffer;
	frame_increment_ = float(buffer.sample_info->SR) / buffer.sample_rate;
	sample_loop_ = data.toggle_loop;
	analysis_data_ = analysis_data;
	resets_ = &block_traverser.get_resets();

	const auto transpose = plugin_->env_grain_transpose().search_vec(data.env_grain_transpose, *block_positions_);
	const auto pitch = plugin_->env_pitch().search_vec(data.env_pitch, *block_positions_) + data.slider_pitch->value;

	ff_ = blink::math::convert::p_to_ff(pitch);

	const auto size_in_ms = convert::linear_to_ms(plugin_->env_grain_size().search_vec(data.env_grain_size, *block_positions_));
	const auto size_in_samples = convert::ms_to_samples(size_in_ms, buffer_->sample_info->SR);

	size_ = blink::math::convert::p_to_ff(blink::math::convert::ff_to_p(size_in_samples) - transpose);
	uniformity_ = plugin_->env_uniformity().search_vec(data.env_uniformity, *block_positions_);
	scale_ = plugin_->chord_harmonics_scale().search_vec(data.chord_harmonics_scale, *block_positions_);
	spread_ = plugin_->env_harmonics_spread().search_vec(data.env_harmonics_spread, *block_positions_);
}

float Controller::get_harmonic_ratio(int index, int harmonic) const
{
	return snap_ratio_to_scale(index, 1.0f + (float(harmonic + 1) * spread_[index]));
}

float Controller::snap_ratio_to_scale(int index, float ff) const
{
	const auto scale = scale_[index];

	if (scale == 0) return ff;

	const auto pitch = blink::math::convert::ff_to_p(ff);

	return blink::math::convert::p_to_ff(snap_pitch_to_scale(pitch, scale));
}

float Controller::snap_pitch_to_scale(float pitch, std::int32_t scale) const
{
	auto octaves = int(std::floor(pitch / 12));
	auto octave_offset = (octaves * 12);

	auto note = int(blink::math::wrap(pitch, 12.0f));

	if (blink::bits::check(scale, note)) return float(note + octave_offset);

	int offset = 1;

	for (int i = 0; i < 6; i++)
	{
		int check = blink::math::wrap(note + offset, 12);

		if (blink::bits::check(scale, check)) return float(check + octave_offset);

		check = blink::math::wrap(note - offset, 12);

		if (blink::bits::check(scale, check)) return float(check + octave_offset);

		offset++;
	}

	return pitch;
}
