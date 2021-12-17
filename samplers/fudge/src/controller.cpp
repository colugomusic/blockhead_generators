#include "controller.h"
#include "audio_data.h"
#include "plugin.h"
#include "convert.h"
#include <blink/dsp.hpp>

namespace fudge {

Controller::Controller(const Plugin* plugin)
	: plugin_(plugin)
{

}

void Controller::process(
	const AudioData& data,
	const blink_SamplerBuffer& buffer,
	const SampleAnalysis* analysis_data,
	const blink::Traverser& block_traverser,
	const blink::BlockPositions& block_positions,
	blink_SR SR)
{
	SR_ = SR;
	block_positions_ = &block_positions;

	position_traverser_.get_positions(
		data.sliders.speed.value(),
		&data.envelopes.speed.data(),
		data.warp_points,
		block_traverser,
		data.sliders.sample_offset,
		kFloatsPerDSPVector,
		nullptr,
		&sample_positions_,
		nullptr);

	sample_positions_ /= float(buffer.song_rate) / buffer.sample_info->SR;

	sample_data_ = blink::SampleData(buffer.sample_info, buffer.channel_mode);

	if (data.toggles.reverse)
	{
		sample_positions_ = std::int32_t(buffer.sample_info->num_frames - 1) - sample_positions_;
	}

	if (data.toggles.loop)
	{
		sample_positions_ = blink::math::wrap(sample_positions_, float(buffer.sample_info->num_frames));
	}

	buffer_ = &buffer;
	frame_increment_ = float(buffer.sample_info->SR) / SR;
	sample_loop_ = data.toggles.loop;
	analysis_data_ = analysis_data;
	resets_ = &block_traverser.get_resets();

	const auto transpose = data.envelopes.grain_transpose.search_vec(*block_positions_);
	const auto pitch = data.envelopes.pitch.search_vec(*block_positions_) + data.sliders.pitch.value();

	ff_ = blink::math::convert::p_to_ff(pitch);

	const auto size_in_ms = convert::linear_to_ms(data.envelopes.grain_size.search_vec(*block_positions_));
	const auto size_in_samples = convert::ms_to_samples(size_in_ms, buffer_->sample_info->SR);

	size_ = blink::math::convert::p_to_ff(blink::math::convert::ff_to_p(size_in_samples) - transpose);
	uniformity_ = data.envelopes.grain_uniformity.search_vec(*block_positions_);
	scale_ = data.chords.scale.search_vec(*block_positions_);
	spread_ = data.envelopes.harmonics_spread.search_vec(*block_positions_);
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

	return blink::math::convert::p_to_ff(blink::snap_pitch_to_scale(pitch, scale));
}

}
