#include "controller.h"
#include "audio_data.h"
#include "convert.h"
#include <blink/dsp.hpp>
#include <blink/search.hpp>
#include <blink/transform/stretch.hpp>

using namespace blink;

namespace fudge {

void Controller::process(
	const AudioData& data,
	const blink_SamplerVaryingData& varying,
	const blink_SamplerUniformData& uniform,
	const SampleAnalysis* analysis_data,
	const blink::Traverser& block_traverser,
	const blink::BlockPositions& block_positions,
	int64_t data_offset,
	blink_SR SR)
{
	SR_              = SR;
	block_positions_ = &block_positions;
	data_offset_     = data_offset;

	transform::Stretch::Config config;

	config.unit_state_id = uniform.base.id;
	config.env.speed = data.env.speed.data;
	config.option.reverse = data.option.reverse_mode.data;
	config.sample_offset = data.slider.sample_offset.value;
	config.speed = data.slider.speed.value;
	config.warp_points = uniform.base.warp_points;
	config.outputs.derivatives.sped = false;
	config.outputs.derivatives.warped = false;

	stretch_transformer_(config, block_positions, kFloatsPerDSPVector);

	sample_positions_ = stretch_transformer_.get_reversed_positions().positions;
	sample_positions_ /= float(uniform.base.song_rate.value) / varying.sample_info->SR.value;

	sample_data_ = blink::SampleData(varying.sample_info, uniform.channel_mode);

	if (data.toggle.reverse.value)
	{
		sample_positions_ = std::int32_t(varying.sample_info->num_frames.value - 1) - sample_positions_;
	}

	if (data.toggle.loop.value)
	{
		sample_positions_ = blink::math::wrap(sample_positions_, float(varying.sample_info->num_frames.value));
	}

	varying_ = &varying;
	frame_increment_ = float(varying.sample_info->SR.value) / SR.value;
	sample_loop_ = data.toggle.loop.value;
	analysis_data_ = analysis_data;
	resets_ = &block_traverser.get_resets();

	const auto transpose = blink::search::vec(data.env.grain_transpose, *block_positions_);
	const auto pitch     = blink::search::vec(data.env.pitch, *block_positions_) + data.slider.pitch.value;

	ff_ = blink::math::convert::p_to_ff(pitch);

	const auto size_in_ms      = convert::linear_to_ms(blink::search::vec(data.env.grain_size, *block_positions_));
	const auto size_in_samples = convert::ms_to_samples(size_in_ms, varying.sample_info->SR.value);

	size_ = blink::math::convert::p_to_ff(blink::math::convert::ff_to_p(size_in_samples) - transpose);
	uniformity_ = blink::search::vec(data.env.grain_uniformity, *block_positions_);
	scale_      = blink::search::vec(data.chord.scale, *block_positions_);
	spread_     = blink::search::vec(data.env.harmonics_spread, *block_positions_);
}

float Controller::get_harmonic_ratio(int index, int harmonic) const {
	return snap_ratio_to_scale(index, 1.0f + (float(harmonic + 1) * spread_[index]));
}

float Controller::snap_ratio_to_scale(int index, float ff) const {
	const auto scale = scale_[index]; 
	if (scale == 0) return ff; 
	const auto pitch = blink::math::convert::ff_to_p(ff); 
	return blink::math::convert::p_to_ff(blink::snap_pitch_to_scale(pitch, scale));
}

} // fudge
