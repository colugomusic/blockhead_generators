#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <chrono>
#include "tract.h"

Tract::Step::Step()
	: a_{ 0.0 }
	, diameter_{ 0.0 }
	, rest_diameter_{ 0.0 }
	, target_diameter_{ 0.0 }
	, new_diameter_{ 0.0 }
	, L_{ 0.0 }
	, R_{ 0.0 }
	, junction_output_L_{ 0.0 }
	, junction_output_R_{ 0.0 }
	, reflection_{ 0.0 }
	, new_reflection_{ 0.0 }
	, nose_a_{ 0.0 }
	, nose_L_{ 0.0 }
	, nose_R_{ 0.0 }
	, nose_diameter_{ 0.0 }
	, nose_junction_output_L_{ 0.0 }
	, nose_junction_output_R_{ 0.0 }
	, nose_reflection_{ 0.0 }
{
	reset();
}

void Tract::Step::reset()
{
	for (int i = 0; i < N; i++)
	{
		auto diameter = 0.0;

		if (i < 7.0 * N / 44.0 - 0.5)
		{
			diameter = 0.6;
		}
		else if (i < 12.0 * N / 44.0)
		{
			diameter = 1.1;
		}
		else
		{
			diameter = 1.0;
		}

		diameter_[i] = rest_diameter_[i] = target_diameter_[i] = new_diameter_[i] = diameter;
	}

	for (int i = 0; i < NOSE_LENGTH; i++)
	{
		double diameter;

		auto d = 2.0 * (double(i) / NOSE_LENGTH);

		if (d < 1.0)
		{
			diameter = 0.4 + 1.6 * d;
		}
		else
		{
			diameter = 0.5 + 1.5 * (2.0 - d);
		}

		diameter = std::min(diameter, 1.9);

		nose_diameter_[i] = diameter;
	}

	new_reflection_L_ = new_reflection_R_ = new_reflection_nose_ = 0.0;

	calculate_reflections();
	calculate_nose_reflections();
	nose_diameter_[0] = velum_target_;
}

void Tract::Step::configure(const Input& input)
{
	set_rest_diameter(input);

	target_diameter_ = rest_diameter_;

	if (input.index > NOSE_START && input.diameter < -NOSE_OFFSET)
	{
		velum_target_ = 0.4;
	}

	if (input.diameter < -0.85 - NOSE_OFFSET) return;

	auto diameter = input.diameter - 0.3;

	if (diameter < 0) diameter = 0.0;

	auto width = 2.0;

	if (input.index < 25)
	{
		width = 10.0;
	}
	else if (input.index >= TIP_START)
	{
		width = 5.0;
	}
	else
	{
		width = 10.0 - 5.0 * (input.index - 25) / (TIP_START - 25);
	}

	if (input.index >= 2 && input.index < N && diameter < 3)
	{
		auto int_index = int(input.index);

		for (int i = -int(std::ceil(width)) - 1; i < width + 1; i++)
		{
			if (int_index + i < 0 || int_index + i >= N) continue;

			auto rel_pos = (int_index + i) - input.index;
			rel_pos = std::abs(rel_pos) - 0.5;
			double shrink;
			if (rel_pos <= 0)
			{
				shrink = 0.0;
			}
			else if (rel_pos > width)
			{
				shrink = 1.0;
			}
			else
			{
				shrink = 0.5 * (1.0 - std::cos(M_PI * rel_pos / width));
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
		const auto t = 1.1 * M_PI * (input.tongue.index - i) / (TIP_START - BLADE_START);
		const auto fixed_tongue_diameter = 2.0 + (input.tongue.diameter - 2.0) / 1.5;

		auto curve = (1.5 - fixed_tongue_diameter + grid_offset_) * std::cos(t);
		//auto curve = (1.5 - fixed_tongue_diameter) * std::cos(t);

		if (i == BLADE_START - 2 || i == LIP_START - 1)
		{
			curve *= 0.8;
		}

		if (i == BLADE_START || i == LIP_START - 2)
		{
			curve *= 0.94;
		}

		rest_diameter_[i] = 1.5 - curve;
	}
}

double Tract::Step::operator()(int SR, double lambda, const Input& input)
{
	configure(input);
	process_transients(SR);
	add_turbulence_noise(input);

	junction_output_R_[0] = L_[0] * glottal_reflection_ + input.glottal_output;
	junction_output_L_[N] = R_[N - 1] * lip_reflection_;

	for (int i = 1; i < N; i++)
	{
		const auto r = reflection_[i] * (1.0 - lambda) + new_reflection_[i] * lambda;
		const auto w = r * (R_[i - 1] + L_[i]);

		junction_output_R_[i] = R_[i - 1] - w;
		junction_output_L_[i] = L_[i] + w;

		if (junction_output_R_[i] > 1.0 || junction_output_R_[i] < -1.0)
		{
			int x = 0;
		}
	}

	const int i = NOSE_START;

	auto r = new_reflection_L_ * (1.0 - lambda) + reflection_L_ * lambda;
	junction_output_L_[i] = r * R_[i - 1] + (1.0 + r) * (nose_L_[0] + L_[i]);
	r = new_reflection_R_ * (1.0 - lambda) + reflection_R_ * lambda;
	junction_output_R_[i] = r * L_[i] + (1.0 + r) * (R_[i - 1] + nose_L_[0]);
	r = new_reflection_nose_ * (1.0 - lambda) + reflection_nose_ * lambda;
	nose_junction_output_R_[0] = r * nose_L_[0] + (1.0 + r) * (L_[i] + R_[i - 1]);

	for (int i = 0; i < N; i++)
	{
		R_[i] = junction_output_R_[i] * 0.999;
		L_[i] = junction_output_L_[i + 1] * 0.999;
	}

	const auto lip_output = R_[N - 1];

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

	const auto nose_output = nose_R_[NOSE_LENGTH - 1];

	return lip_output + nose_output;
}

static constexpr float lambda1_fn(int i) { return i / static_cast<float>(kFloatsPerDSPVector); }
static constexpr float lambda2_fn(int i) { return (float(i) + 0.5f) / static_cast<float>(kFloatsPerDSPVector); }

ml::DSPVector Tract::operator()(int SR, const Input& input)
{
	ml::DSPVector out;

	constexpr ml::DSPVector lambda1{ lambda1_fn };
	constexpr ml::DSPVector lambda2{ lambda2_fn };

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

		out[i] += float(step_(SR, double(lambda1[i]), step_input));
		out[i] += float(step_(SR, double(lambda2[i]), step_input));
	}

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
	t.time_alive = 0.0;
	t.life_time = 0.2;
	t.strength = 0.3;
	t.exponent = 200.0;
	
	transients_.list[transients_.count++] = t;
}

void Tract::Step::process_transients(int SR)
{
	for (int i = 0; i < transients_.count; i++)
	{
		auto& transient = transients_.list[i];
		const auto amplitude = transient.strength * std::pow(2.0, -transient.exponent * transient.time_alive);

		R_[transient.position] += amplitude / 2.0;
		L_[transient.position] += amplitude / 2.0;

		transient.time_alive += 1.0 / (SR * 2.0);
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
	if (input.index < 2 || input.index >= N - 2) return;
	if (input.diameter <= 0) return;
	if (input.fricative_intensity <= 0.0001) return;

	add_turbulence_noise_at_index(0.66 * input.fricative_noise * input.fricative_intensity, input.index, input.diameter);
}

void Tract::Step::add_turbulence_noise_at_index(double noise, double index, double diameter)
{
	const auto i = int(std::floor(index));
	const auto delta = index - i;

	const auto thinness0 = std::clamp(8.0 * (0.7 - diameter), 0.0, 1.0);
	const auto openness = std::clamp(30.0 * (diameter - 0.3), 0.0, 1.0);
	const auto noise0 = noise * (1.0 - delta) * thinness0 * openness;
	const auto noise1 = noise * delta * thinness0 * openness;

	R_[i + 1] += noise0 / 2;
	L_[i + 1] += noise0 / 2;
	R_[i + 2] += noise1 / 2;
	L_[i + 2] += noise1 / 2;
}

void Tract::Step::reshape_tract(double delta_time)
{
	auto amount = delta_time * MOVEMENT_SPEED;
	auto new_last_obstruction = -1;

	auto move_towards = [](double current, double target, double amount_up, double amount_down)
	{
		if (current < target) return std::min(current + amount_up, target);
		else return std::max(current - amount_down, target);
	};

	for (int i = 0; i < N; i++)
	{
		auto diameter = diameter_[i];
		auto target_diameter = target_diameter_[i];

		if (diameter <= 0)
		{
			new_last_obstruction = i;
		}

		double slow_return;

		if (i < NOSE_START)
		{
			slow_return = 0.6;
		}
		else if (i >= TIP_START)
		{
			slow_return = 1.0;
		}
		else
		{
			slow_return = 0.6 + 0.4 * (double(i) - NOSE_START) / (TIP_START - NOSE_START);
		}

		diameter_[i] = move_towards(diameter, target_diameter, slow_return * amount, 2.0 * amount);
	}

	if (last_obstruction_ > -1 && new_last_obstruction == -1 && nose_a_[0] < 0.05)
	{
		add_transient(last_obstruction_);
	}

	last_obstruction_ = new_last_obstruction;

	nose_diameter_[0] = move_towards(nose_diameter_[0], velum_target_, amount * 0.25, amount * 0.1);
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

		if (a_[i] < 0.000001)
		{
			new_reflection_[i] = 0.999;
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

	new_reflection_L_ = (2.0 * a_[NOSE_START] - sum) / sum;
	new_reflection_R_ = (2.0 * a_[NOSE_START + 1] - sum) / sum;
	new_reflection_nose_ = (2.0 * nose_a_[0] - sum) / sum;
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
	reshape_tract(double(kFloatsPerDSPVector) / SR);
	calculate_reflections();
}

