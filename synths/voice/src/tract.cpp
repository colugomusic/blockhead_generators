#include "tract.h"

Tract::Tract()
	: rng_dist_(0.0f, 1.0f)
	, a_{ 0.0f }
	, max_amplitude_{ 0.0f }
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
	, nose_max_amplitude_{ 0.0f }
	, nose_junction_output_L_{ 0.0f }
	, nose_junction_output_R_{ 0.0f }
	, nose_reflection_{ 0.0f }
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
			diameter = 1.5f;
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

static constexpr float lambda1_fn(int i) { return i / static_cast<float>(kFloatsPerDSPVector); }
static constexpr float lambda2_fn(int i) { return (float(i) + 0.5f) / static_cast<float>(kFloatsPerDSPVector); }

ml::DSPVector Tract::operator()(int SR, const ml::DSPVector& glottal_output, const ml::DSPVector& fricative_noise)
{
	ml::DSPVector out;

	constexpr ml::DSPVector lambda1{ lambda1_fn };
	constexpr ml::DSPVector lambda2{ lambda2_fn };

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		out[i] += step(SR, glottal_output[i], fricative_noise[i], lambda1[i]);
		out[i] += step(SR, glottal_output[i], fricative_noise[i], lambda2[i]);
	}

	finish_block(SR);

	return out;
}

void Tract::process_transients()
{

	/*

			for (var i = 0; i < this.transients.length; i++)
		{
			var trans = this.transients[i];
			var amplitude = trans.strength * Math.pow(2, -trans.exponent * trans.timeAlive);
			this.R[trans.position] += amplitude/2;
			this.L[trans.position] += amplitude/2;
			trans.timeAlive += 1.0/(sampleRate*2);
		}
		for (var i=this.transients.length-1; i>=0; i--)
		{
			var trans = this.transients[i];
			if (trans.timeAlive > trans.lifeTime)
			{
				this.transients.splice(i,1);
			}
		}

	*/
}

void Tract::add_turbulence_noise(float noise)
{
	/*
	        for (var j=0; j<UI.touchesWithMouse.length; j++)
        {
            var touch = UI.touchesWithMouse[j];
            if (touch.index<2 || touch.index>Tract.n) continue;
            if (touch.diameter<=0) continue;            
            var intensity = touch.fricative_intensity;
            if (intensity == 0) continue;
            this.addTurbulenceNoiseAtIndex(0.66*turbulenceNoise*intensity, touch.index, touch.diameter);
        }
	
	*/
}

float Tract::step(int SR, float glottal_output, float noise, float lambda)
{
	const auto update_amplitudes = random() < 0.1f;

	process_transients();
	add_turbulence_noise(noise);

	junction_output_R_[0] = L_[0] * glottal_reflection_ + glottal_output;
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
	junction_output_R_[i] = r * L_[i] + (1.0f - r) * (R_[i - 1] + nose_L_[0]);
	r = new_reflection_nose_ * (1.0f - lambda) + reflection_nose_ * lambda;
	nose_junction_output_R_[0] = r * nose_L_[0] + (1.0f + r) * (L_[i] + R_[i - 1]);

	for (int i = 0; i < N; i ++)
	{
		R_[i] = junction_output_R_[i] * 0.999f;
		L_[i] = junction_output_L_[i + 1] * 0.999f;

		if (update_amplitudes)
		{
			const auto amplitude = std::abs(R_[i] + L_[i]);

			if (amplitude > max_amplitude_[i])
			{
				max_amplitude_[i] = amplitude;
			}
			else
			{
				max_amplitude_[i] *= 0.999f;
			}
		}
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

		if (update_amplitudes)
		{
			const auto amplitude = std::abs(nose_R_[i] + nose_L_[i]);

			if (amplitude > nose_max_amplitude_[i])
			{
				nose_max_amplitude_[i] = amplitude;
			}
			else
			{
				nose_max_amplitude_[i] *= 0.999f;
			}
		}
	}

	const auto nose_output = nose_R_[NOSE_LENGTH - 1];

	return lip_output + nose_output;
}

void Tract::reshape_tract(float delta_time)
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

		if (diameter <= 0)
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
			slow_return = 0.6f + 0.4f * (i - NOSE_START) / (TIP_START - NOSE_START);
		}

		diameter_[i] = move_towards(diameter, target_diameter, slow_return * amount, 2.0f * amount);
	}

	if (last_obstruction_ > -1 && new_last_obstruction == -1 && nose_a_[0] < 0.05f)
	{
		// TODO: add transient
	}

	last_obstruction_ = new_last_obstruction;

	amount = delta_time * MOVEMENT_SPEED;

	nose_diameter_[0] = move_towards(nose_diameter_[0], velum_target_, amount * 0.25f, amount * 0.1f);
	nose_a_[0] = nose_diameter_[0] * nose_diameter_[0];
}

void Tract::calculate_reflections()
{
	for (int i = 0; i < N; i++)
	{
		a_[i] = diameter_[i] * diameter_[i];
	}

	for (int i = 1; i < N; i++)
	{
		reflection_[i] = new_reflection_[i];
		if (a_[i] == 0)
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

	new_reflection_L_ = (2 * a_[NOSE_START] - sum) / sum;
	new_reflection_R_ = (2 * a_[NOSE_START + 1] - sum) / sum;
	new_reflection_nose_ = (2 * nose_a_[0] - sum) / sum;
}

void Tract::calculate_nose_reflections()
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

void Tract::finish_block(int SR)
{
	reshape_tract(float(kFloatsPerDSPVector) / SR);
	calculate_reflections();
}