#include "particle.h"
#include "controller.h"
#include <blink/math.hpp>
#include <cassert>

namespace freeze {

Particle::Particle(const Controller& controller, std::function<float(int, std::size_t, float)> read)
	: controller_(&controller)
	, read_(read)
{
}

LR Particle::process(int vector_index)
{
	LR out;

	if (queue_reset_)
	{
		reset(vector_index);
	}
	else
	{
		const auto half_period = grains_[flip_flop_].period / 2;

		if (trigger_timer_ >= half_period)
		{
			trigger_timer_ -= half_period;
			trigger_next_grain(vector_index);
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
		auto overall_amp = grain.frame_amp * final_duck;

		if (overall_amp > 0.f)
		{
			const auto LR = read_stereo_frame(vector_index, grain);

			L = LR.L * overall_amp;
			R = LR.R * overall_amp;
		}

		grain.frame++;

		if (grain.frame >= grain.size)
		{
			grain.on = false;
		}

		out.L += L;
		out.R += R;
	}

	trigger_timer_++;

	return out;
}

LR Particle::read_stereo_frame(int vector_index, const Grain& grain) const
{
	const auto pos = grain.frame * grain.ff;

	if (pos < 0)
	{
		return { 0.0f, 0.0f };
	}

	if (pos >= grain.edge)
	{
		return { 0.0f, 0.0f };
	}

	auto amp = 1.0f;

	if (pos >= grain.edge - 512)
	{
		amp = 1.0f - blink::math::inverse_lerp(float(grain.edge - 512), float(grain.edge), pos);
	}

	const auto L = read_(vector_index, 0, pos) * amp;
	const auto R = read_(vector_index, 1, pos) * amp;

	return { L, R };
}

void Particle::clear()
{
	grains_[0].on = false;
	grains_[1].on = false;

	first_grain_ = true;
	trigger_timer_ = 0.0f;
}

void Particle::reset(int index)
{
	queue_reset_ = false;
	clear();
	trigger_next_grain(index);
}

void Particle::trigger_next_grain(int index)
{
	constexpr auto MAX_WINDOW_SIZE = 4096.0f;

	flip_flop_ = flip_flop_ == 0 ? 1 : 0;

	auto ff = controller_->ff()[index];
	auto size = controller_->size()[index];
	auto period = size;
	const auto fade_in = true;// !first_grain_ || ff != 1.0f;

	first_grain_ = false;

	const auto& buffer = controller_->buffer();
	const auto frames_available = buffer.frames_available();
	const auto edge = frames_available;

	grains_[flip_flop_].on = true;
	grains_[flip_flop_].fade_in = fade_in;
	grains_[flip_flop_].ff = ff;
	grains_[flip_flop_].size = size;
	grains_[flip_flop_].period = period;
	grains_[flip_flop_].edge = edge;
	grains_[flip_flop_].window = std::min(MAX_WINDOW_SIZE, std::floor(size / 3));
	grains_[flip_flop_].frame = 0.f;
	grains_[flip_flop_].frame_amp = fade_in ? 0.f : 1.f;
	grains_[flip_flop_].duck = 1.f;
}

} // freeze