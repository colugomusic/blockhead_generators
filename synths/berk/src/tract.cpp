#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <chrono>
#include "tract.h"

Tract::Step::Step()
	: a_{ 0.0f }
	, diameter_{ 0.0f }
	, rest_diameter_{ 0.0f }
	, target_diameter_{ 0.0f }
	, new_diameter_{ 0.0f }
	, L_{ 0.0f }
	, R_{ 0.0f }
	, junction_output_L_{ 0.0f }
	, junction_output_R_{ 0.0f }
	, reflection_{ 0.0f }
	, new_reflection_{ 0.0f }
	, nose_a_{ 0.0f }
	, nose_L_{ 0.0f }
	, nose_R_{ 0.0f }
	, nose_diameter_{ 0.0f }
	, nose_junction_output_L_{ 0.0f }
	, nose_junction_output_R_{ 0.0f }
	, nose_reflection_{ 0.0f }
{
	reset();
}

void Tract::Step::reset()
{
	for (int i = 0; i < N; i++)
	{
		auto diameter = 0.0f;

		if (i < 7.0f * N / 44.0f - 0.5f)
		{
			diameter = 0.6f;
		}
		else if (i < 12.0f * N / 44.0f)
		{
			diameter = 1.1f;
		}
		else
		{
			diameter = 1.0f;
		}

		diameter_[i] = rest_diameter_[i] = target_diameter_[i] = new_diameter_[i] = diameter;
	}

	for (int i = 0; i < NOSE_LENGTH; i++)
	{
		float diameter;

		auto d = 2.0f * (float(i) / NOSE_LENGTH);

		if (d < 1.0f)
		{
			diameter = 0.4f + 1.6f * d;
		}
		else
		{
			diameter = 0.5f + 1.5f * (2.0f - d);
		}

		diameter = std::min(diameter, 1.9f);

		nose_diameter_[i] = diameter;
	}

	new_reflection_L_ = new_reflection_R_ = new_reflection_nose_ = 0.0f;

	calculate_reflections();
	calculate_nose_reflections();
	nose_diameter_[0] = velum_target_;
}

void Tract::Step::configure(const Input& input)
{
	set_rest_diameter(input);

	target_diameter_ = rest_diameter_;

	velum_target_ = 0.01f;

	if (input.index > NOSE_START && input.diameter < -NOSE_OFFSET)
	{
		velum_target_ = 0.4f;
	}

	if (input.diameter < -0.85f - NOSE_OFFSET) return;

	auto diameter = input.diameter - 0.3f;

	if (diameter < 0.0f) diameter = 0.0f;

	auto width = 2.0f;

	if (input.index < 25.0f)
	{
		width = 10.0f;
	}
	else if (input.index >= TIP_START)
	{
		width = 5.0f;
	}
	else
	{
		width = 10.0f - 5.0f * (input.index - 25) / (TIP_START - 25);
	}

	if (input.index >= 2 && input.index < N && diameter < 3)
	{
		auto int_index = int(input.index);

		for (int i = -int(std::ceil(width)) - 1; i < width + 1; i++)
		{
			if (int_index + i < 0 || int_index + i >= N) continue;

			auto rel_pos = (int_index + i) - input.index;

			rel_pos = std::abs(rel_pos) - 0.5f;

			float shrink;

			if (rel_pos <= 0.0f)
			{
				shrink = 0.0f;
			}
			else if (rel_pos > width)
			{
				shrink = 1.0f;
			}
			else
			{
				shrink = 0.5f * (1.0f - std::cos(float(float(M_PI)) * rel_pos / width));
			}

			if (diameter < target_diameter_[int_index + i])
			{
				target_diameter_[int_index + i] = diameter + (target_diameter_[int_index + i] - diameter) * shrink;
			}
		}
	}
}

void Tract::Step::set_rest_diameter(const Input& input)
{
	for (int i = BLADE_START; i < LIP_START; i++)
	{
		const auto t = 1.1f * float(M_PI) * (input.tongue.index - i) / (TIP_START - BLADE_START);
		const auto fixed_tongue_diameter = 2.0f + (input.tongue.diameter - 2.0f) / 1.5f;

		auto curve = (1.5f - fixed_tongue_diameter + grid_offset_) * std::cos(t);
		//auto curve = (1.5 - fixed_tongue_diameter) * std::cos(t);

		if (i == BLADE_START - 2 || i == LIP_START - 1)
		{
			curve *= 0.8f;
		}

		if (i == BLADE_START || i == LIP_START - 2)
		{
			curve *= 0.94f;
		}

		rest_diameter_[i] = 1.5f - curve;
	}
}

void Tract::Step::operator()(int SR, float lambda, const Input& input, float* lip_out, float* nose_out)
{
	configure(input);
	process_transients(SR);
	add_turbulence_noise(input);

	junction_output_R_[0] = L_[0] * glottal_reflection_ + input.glottal_output;
	junction_output_L_[N] = R_[N - 1] * lip_reflection_;

	for (int i = 1; i < N; i++)
	{
		const auto r = reflection_[i] * (1.0f - lambda) + new_reflection_[i] * lambda;
		const auto w = r * (R_[i - 1] + L_[i]);

		junction_output_R_[i] = R_[i - 1] - w;
		junction_output_L_[i] = L_[i] + w;
	}

	const int i = NOSE_START;

	auto r = new_reflection_L_ * (1.0f - lambda) + reflection_L_ * lambda;
	junction_output_L_[i] = r * R_[i - 1] + (1.0f + r) * (nose_L_[0] + L_[i]);
	r = new_reflection_R_ * (1.0f - lambda) + reflection_R_ * lambda;
	junction_output_R_[i] = r * L_[i] + (1.0f + r) * (R_[i - 1] + nose_L_[0]);
	r = new_reflection_nose_ * (1.0f - lambda) + reflection_nose_ * lambda;
	nose_junction_output_R_[0] = r * nose_L_[0] + (1.0f + r) * (L_[i] + R_[i - 1]);

	for (int i = 0; i < N; i++)
	{
		R_[i] = junction_output_R_[i] * 0.999f;
		L_[i] = junction_output_L_[i + 1] * 0.999f;
	}

	*lip_out = R_[N - 1];

	nose_junction_output_L_[NOSE_LENGTH] = nose_R_[NOSE_LENGTH - 1] * lip_reflection_;

	for (int i = 1; i < NOSE_LENGTH; i++)
	{
		const auto w = nose_reflection_[i] * (nose_R_[i - 1] + nose_L_[i]);

		nose_junction_output_R_[i] = nose_R_[i - 1] - w;
		nose_junction_output_L_[i] = nose_L_[i] + w;
	}

	for (int i = 0; i < NOSE_LENGTH; i++)
	{
		nose_R_[i] = nose_junction_output_R_[i] * fade_;
		nose_L_[i] = nose_junction_output_L_[i + 1] * fade_;
	}

	*nose_out = nose_R_[NOSE_LENGTH - 1];
}

static constexpr float lambda1_fn(int i) { return i / static_cast<float>(kFloatsPerDSPVector); }
static constexpr float lambda2_fn(int i) { return (float(i) + 0.5f) / static_cast<float>(kFloatsPerDSPVector); }

ml::DSPVector Tract::operator()(int SR, const Input& input)
{
	ml::DSPVector out;

	constexpr ml::DSPVector lambda1{ lambda1_fn };
	constexpr ml::DSPVector lambda2{ lambda2_fn };

	ml::DSPVector lip;
	ml::DSPVector nose;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		Step::Input step_input;

		step_input.diameter = input.diameter[i];
		step_input.fricative_intensity = input.fricative_intensity[i];
		step_input.fricative_noise = input.fricative_noise[i];
		step_input.glottal_output = input.glottal_output[i];
		step_input.index = input.index[i];
		step_input.tongue.diameter = input.tongue.diameter[i];
		step_input.tongue.index = input.tongue.index[i];

		step_(SR, float(lambda1[i]), step_input, &(lip[i]), &(nose[i]));
		step_(SR, float(lambda1[i]), step_input, &(lip[i]), &(nose[i]));
	}

	out = lip + nose;

	step_.finish_block(SR);

	ml::validate(out);

	out = ml::clamp(out, ml::DSPVector(-1.0), ml::DSPVector(1.0));

	return out;
}

void Tract::reset()
{
	step_.reset();
}

void Tract::Step::add_transient(int position)
{
	if (transients_.count == transients_.list.size()) return;

	Transients::Transient t;

	t.position = position;
	t.time_alive = 0.0f;
	t.life_time = 0.2f;
	t.strength = 0.3f;
	t.exponent = 200.0f;
	
	transients_.list[transients_.count++] = t;
}

void Tract::Step::process_transients(int SR)
{
	for (int i = 0; i < transients_.count; i++)
	{
		auto& transient = transients_.list[i];
		const auto amplitude = transient.strength * std::pow(2.0f, -transient.exponent * transient.time_alive);

		R_[transient.position] += amplitude / 2.0f;
		L_[transient.position] += amplitude / 2.0f;

		transient.time_alive += 1.0f / (SR * 2.0f);
	}

	for (int i = transients_.count - 1; i >= 0; i--)
	{
		auto& transient = transients_.list[i];

		if (transient.time_alive > transient.life_time)
		{
			transients_.count--;
		}
	}
}

void Tract::Step::add_turbulence_noise(const Input& input)
{
	if (input.index < 2.0f || input.index >= N - 2) return;
	if (input.diameter <= 0.0f) return;
	if (input.fricative_intensity <= 0.0001f) return;

	add_turbulence_noise_at_index(0.66f * input.fricative_noise * input.fricative_intensity, input.index, input.diameter);
}

void Tract::Step::add_turbulence_noise_at_index(float noise, float index, float diameter)
{
	const auto i = int(std::floor(index));
	const auto delta = index - i;

	const auto thinness0 = std::clamp(8.0f * (0.7f - diameter), 0.0f, 1.0f);
	const auto openness = std::clamp(30.0f * (diameter - 0.3f), 0.0f, 1.0f);
	const auto noise0 = noise * (1.0f - delta) * thinness0 * openness;
	const auto noise1 = noise * delta * thinness0 * openness;

	R_[i + 1] += noise0 / 2;
	L_[i + 1] += noise0 / 2;
	R_[i + 2] += noise1 / 2;
	L_[i + 2] += noise1 / 2;
}

void Tract::Step::reshape_tract(float delta_time)
{
	auto amount = delta_time * MOVEMENT_SPEED;
	auto new_last_obstruction = -1;

	auto move_towards = [](float current, float target, float amount_up, float amount_down)
	{
		if (current < target) return std::min(current + amount_up, target);
		else return std::max(current - amount_down, target);
	};

	for (int i = 0; i < N; i++)
	{
		auto diameter = diameter_[i];
		auto target_diameter = target_diameter_[i];

		if (diameter <= 0.0f)
		{
			new_last_obstruction = i;
		}

		float slow_return;

		if (i < NOSE_START)
		{
			slow_return = 0.6f;
		}
		else if (i >= TIP_START)
		{
			slow_return = 1.0f;
		}
		else
		{
			slow_return = 0.6f + 0.4f * (float(i) - NOSE_START) / (TIP_START - NOSE_START);
		}

		diameter_[i] = move_towards(diameter, target_diameter, slow_return * amount, 2.0f * amount);
	}

	if (last_obstruction_ > -1 && new_last_obstruction == -1 && nose_a_[0] < 0.05f)
	{
		add_transient(last_obstruction_);
	}

	last_obstruction_ = new_last_obstruction;

	nose_diameter_[0] = move_towards(nose_diameter_[0], velum_target_, amount * 0.25f, amount * 0.1f);
	nose_a_[0] = nose_diameter_[0] * nose_diameter_[0];
}

void Tract::Step::calculate_reflections()
{
	for (int i = 0; i < N; i++)
	{
		a_[i] = diameter_[i] * diameter_[i];
	}

	for (int i = 1; i < N; i++)
	{
		reflection_[i] = new_reflection_[i];

		if (a_[i] < 0.000001f)
		{
			new_reflection_[i] = 0.999f;
		}
		else
		{
			new_reflection_[i] = (a_[i - 1] - a_[i]) / (a_[i - 1] + a_[i]);
		}
	}

	reflection_L_ = new_reflection_L_;
	reflection_R_ = new_reflection_R_;
	reflection_nose_ = new_reflection_nose_;

	const auto sum = a_[NOSE_START] + a_[NOSE_START + 1] + nose_a_[0];

	new_reflection_L_ = (2.0f * a_[NOSE_START] - sum) / sum;
	new_reflection_R_ = (2.0f * a_[NOSE_START + 1] - sum) / sum;
	new_reflection_nose_ = (2.0f * nose_a_[0] - sum) / sum;
}

void Tract::Step::calculate_nose_reflections()
{
	for (int i = 0; i < NOSE_LENGTH; i++)
	{
		nose_a_[i] = nose_diameter_[i] * nose_diameter_[i];
	}

	for (int i = 1; i < NOSE_LENGTH; i++)
	{
		nose_reflection_[i] = (nose_a_[i - 1] - nose_a_[i]) / (nose_a_[i - 1] + nose_a_[i]);
	}
}

void Tract::Step::finish_block(int SR)
{
	reshape_tract(float(kFloatsPerDSPVector) / SR);
	calculate_reflections();
}

