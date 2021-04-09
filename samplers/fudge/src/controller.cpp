#include "controller.h"
#include "audio_data.h"
#include "plugin.h"
#include "convert.h"

Controller::Controller(const Fudge* plugin)
	: plugin_(plugin)
{

}

void Controller::process(
	const AudioData& data,
	const blink_SamplerBuffer& buffer,
	const SampleAnalysis* analysis_data,
	const blink::Traverser& block_traverser,
	float prev_pos)
{
	block_positions_ = &block_traverser.get_read_position();

	sample_positions_ =
		position_traverser_.get_positions(
			data.slider_speed->value,
			data.env_speed,
			block_traverser,
			data.slider_sample_offset->value,
			kFloatsPerDSPVector);

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

	const auto transpose = plugin_->env_grain_transpose().search_vec(data.env_grain_transpose, *block_positions_, prev_pos);
	const auto pitch = plugin_->env_pitch().search_vec(data.env_pitch, *block_positions_, prev_pos) + data.slider_pitch->value;

	ff_ = blink::math::convert::p_to_ff(pitch);

	const auto size_in_ms = convert::linear_to_ms(plugin_->env_grain_size().search_vec(data.env_grain_size, *block_positions_, prev_pos));
	const auto size_in_samples = convert::ms_to_samples(size_in_ms, buffer_->sample_info->SR);

	size_ = blink::math::convert::p_to_ff(blink::math::convert::ff_to_p(size_in_samples) - transpose);
	uniformity_ = plugin_->env_uniformity().search_vec(data.env_uniformity, *block_positions_, prev_pos);
}
