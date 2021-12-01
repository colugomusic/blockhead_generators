#include "particle.h"
#include "controller.h"
#include "sample_analysis.h"
#include <blink/math.hpp>
#include <cassert>

namespace fudge {

Particle::Particle(const Controller& controller, int harmonic)
	: controller_(&controller)
	, harmonic_(harmonic)
{
}

ml::DSPVectorArray<2> Particle::process(const ml::DSPVector& amp)
{
	ml::DSPVector vec_L;
	ml::DSPVector vec_R;

	for (int j = 0; j < kFloatsPerDSPVector; j++)
	{
		auto block_pos = controller_->block_positions().positions[j];
		auto local_block_pos = block_pos + controller_->data_offset();

		if (trig_primed_ && local_block_pos >= 0)
		{
			reset(j);
		}
		else
		{
			if (controller_->reset()[j])
			{
				if (local_block_pos < 0)
				{
					trig_primed_ = true;
				}
				else
				{
					reset(j);
				}
			}
			else if(trigger_timer_ >= std::floor(grains_[flip_flop_].size / 2))
			{
				trigger_timer_ = 0.f;
				trigger_next_grain(j, true);
			}
		}

		for (int g = 0; g < 2; g++)
		{
			float L = 0.0f;
			float R = 0.0f;

			auto& grain = grains_[g];

			if (!grain.on) continue;

			if (grain.frame < grain.window)
			{
				if (grain.fade_in)
				{
					grain.frame_amp = blink::math::ease::quadratic::in_out(1.f - ((grain.window - grain.frame) / grain.window));

					float other_grain_duck = 1.f - grain.frame_amp;

					other_grain(g).duck = other_grain_duck;
				}
			}

			float self_duck = 1.f;

			if (grain.frame > grain.size - grain.window)
			{
				self_duck = blink::math::ease::quadratic::in_out(1.f - ((grain.frame - (grain.size - grain.window)) / grain.window));
			}

			auto final_duck = std::min(grain.duck, self_duck);
			auto overall_amp = grain.frame_amp * final_duck * amp[j];

			if (overall_amp > 0.f)
			{
				if (controller_->sample_info().num_channels > 1)
				{
					const auto LR = read_stereo_frame(grain);

					L = LR[0];
					R = LR[1];
				}
				else
				{
					L = read_mono_frame(grain);
					R = L;
				}

				L *= overall_amp;
				R *= overall_amp;
			}

			grain.frame += grain.ff;

			if (grain.frame >= grain.size)
			{
				grain.on = false;
			}

			vec_L[j] += L;
			vec_R[j] += R;
		}

		trigger_timer_ += grains_[flip_flop_].ff;
	}

	return ml::concatRows(vec_L, vec_R);
}

float Particle::read_mono_frame(const Grain& grain) const
{
	const auto pos = grain.pos[0] + grain.frame;

	return pos < 0.f ? 0.f : controller_->sample_data().read_frame_interp(0, pos);
}

std::array<float, 2> Particle::read_stereo_frame(const Grain& grain) const
{
	float L;
	float R;

	const auto& data = controller_->sample_data();

	switch (controller_->sample_data().get_channel_mode())
	{
		default:
		case blink_ChannelMode_Stereo:
		{
			const auto pos_L = grain.pos[0] + grain.frame;
			const auto pos_R = grain.pos[1] + grain.frame;

			L = pos_L < 0.f ? 0.f : data.read_frame_interp(0, pos_L);
			R = pos_R < 0.f ? 0.f : data.read_frame_interp(1, pos_R);

			break;
		}

		case blink_ChannelMode_StereoSwap:
		{
			const auto pos_L = grain.pos[0] + grain.frame;
			const auto pos_R = grain.pos[1] + grain.frame;

			R = pos_L < 0.f ? 0.f : data.read_frame_interp(0, pos_L);
			L = pos_R < 0.f ? 0.f : data.read_frame_interp(1, pos_R);

			break;
		}

		case blink_ChannelMode_Left:
		{
			const auto pos = grain.pos[0] + grain.frame;

			L = pos < 0.f ? 0.f : data.read_frame_interp(0, pos);
			R = L;

			break;
		}

		case blink_ChannelMode_Right:
		{
			const auto pos = grain.pos[1] + grain.frame;

			R = pos < 0.f ? 0.f : data.read_frame_interp(1, pos);
			L = R;

			break;
		}
	}

	return { L, R };
}

void Particle::reset(int index)
{
	grains_[0].on = false;
	grains_[1].on = false;

	trig_primed_ = false;
	trigger_timer_ = 0.0f;
	trigger_next_grain(index, false);
}

float Particle::adjust_channel_pos(int index, int channel, float pos) const
{
	const auto adjust_amount = 1.0f - controller_->uniformity()[index];

	if (adjust_amount < 0.000001f) return pos;

	const auto num_frames = controller_->sample_info().num_frames;

	if (pos > num_frames) return pos;

	const auto analysis_data = controller_->analysis_data();

	if (!analysis_data) return pos;
	if (!analysis_data->done) return pos;

	const auto& other = other_grain(flip_flop_);
	const auto other_pos = other.pos[channel];

	if (other_pos + other.frame < 0.0f) return pos;

	const auto other_pos_floor = int(std::floor(other_pos + other.frame));
	const auto diff = pos - (other_pos + other.frame);
	const auto abs_diff = std::abs(diff);
	const auto& channel_analysis_data = analysis_data->data[channel];
	const auto other_wavecycle = other_pos_floor >= channel_analysis_data.size() ? channel_analysis_data[channel_analysis_data.size() - 1].estimated_size : channel_analysis_data[other_pos_floor].estimated_size;

	const auto a = int(std::floor(float(abs_diff) / other_wavecycle));

	float adjusted_pos;

	if (diff > 0.0f)
	{
		adjusted_pos = (other_pos + other.frame) + (a * other_wavecycle);
	}
	else
	{
		adjusted_pos = (other_pos + other.frame) - (a * other_wavecycle);
	}

	if (adjusted_pos > num_frames)
	{
		adjusted_pos = pos;
	}

	return blink::math::lerp(pos, adjusted_pos, adjust_amount);
}

float Particle::get_mono_position(int index, float pos, bool adjust) const
{
	if (!adjust) return pos;
	if (pos <= 0.0f) return pos;

	return adjust_channel_pos(index, 0, pos);
}

std::array<float, 2> Particle::get_stereo_positions(int index, float pos, bool adjust) const
{
	if (!adjust) return { pos, pos };
	if (pos <= 0.0f) return { pos, pos };
	
	float pos_L;
	float pos_R;

	switch (controller_->sample_data().get_channel_mode())
	{
		default:
		case blink_ChannelMode_Stereo:
		{
			pos_L = adjust_channel_pos(index, 0, pos);
			pos_R = adjust_channel_pos(index, 1, pos);
			break;
		}

		case blink_ChannelMode_Left:
		{
			pos_L = adjust_channel_pos(index, 0, pos);
			pos_R = adjust_channel_pos(index, 0, pos);
			break;
		}

		case blink_ChannelMode_Right:
		{
			pos_L = adjust_channel_pos(index, 1, pos);
			pos_R = adjust_channel_pos(index, 1, pos);
			break;
		}
	}

	if (std::abs(pos_R - pos_L) < 0.01f * controller_->SR())
	{
		pos_R = pos_L;
	}

	return { pos_L, pos_R };
}

void Particle::trigger_next_grain(int index, bool adjust)
{
	constexpr auto MIN_GRAIN_SIZE = 3.0f;
	constexpr auto MAX_WINDOW_SIZE = 4096.0f;

	flip_flop_ = flip_flop_ == 0 ? 1 : 0;

	const auto pos = controller_->position()[index];
	const auto fade_in = pos > 0;

	float pos_L;
	float pos_R;

	if (controller_->sample_info().num_channels > 1)
	{
		const auto positions = get_stereo_positions(index, pos, adjust);

		pos_L = positions[0];
		pos_R = positions[1];
	}
	else
	{
		pos_L = get_mono_position(index, pos, adjust);
		pos_R = pos_L;
	}

	auto ratio = harmonic_ > 0 ? controller_->get_harmonic_ratio(index, harmonic_) : 1.0f;
	auto ff = controller_->ff()[index];
	auto size = std::max(MIN_GRAIN_SIZE, controller_->size()[index] * ff);

	grains_[flip_flop_].on = true;
	grains_[flip_flop_].fade_in = fade_in;
	grains_[flip_flop_].pos[0] = pos_L;
	grains_[flip_flop_].pos[1] = pos_R;
	grains_[flip_flop_].ff = controller_->frame_increment() * ff * ratio;
	grains_[flip_flop_].size = size;
	grains_[flip_flop_].window = std::min(MAX_WINDOW_SIZE, std::floor(size / 3));
	grains_[flip_flop_].frame = 0.f;
	grains_[flip_flop_].frame_amp = fade_in ? 0.f : 1.f;
	grains_[flip_flop_].duck = 1.f;
}

}
