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

LR Particle::process(int vector_index, float* dry)
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
			grain.frame_amp = blink::math::ease::quadratic::in_out(1.f - ((grain.window - grain.frame) / grain.window));

			float other_grain_duck = 1.f - grain.frame_amp;

			other_grain(g).duck = other_grain_duck;
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

		if (grain.dry && grain.frame < grain.window)
		{
			*dry = 1.0f - overall_amp;
		}
	}

	if (!grains_[0].on && !grains_[1].on)
	{
		*dry = 1.0f;
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

	const auto L = read_(vector_index, 0, pos);
	const auto R = read_(vector_index, 1, pos);

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
	constexpr auto MAX_WINDOW_SIZE{ 4096.0f };

	const auto size{ controller_->size()[index] };
	const auto& buffer{ controller_->buffer() };
	const auto frames_available{ buffer.frames_available() };

	if (frames_available < size * 0.5f) return;

	flip_flop_ = 1 - flip_flop_;

	auto ff{ controller_->ff()[index] };

	grains_[flip_flop_].on = true;
	grains_[flip_flop_].dry = first_grain_;
	grains_[flip_flop_].ff = ff;
	grains_[flip_flop_].size = size;
	grains_[flip_flop_].period = size;
	grains_[flip_flop_].window = std::min(MAX_WINDOW_SIZE, std::floor(size / 3));
	grains_[flip_flop_].frame = 0.f;
	grains_[flip_flop_].frame_amp = 0.f;
	grains_[flip_flop_].duck = 1.f;

	first_grain_ = false;

}

} // freeze