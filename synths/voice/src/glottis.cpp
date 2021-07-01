#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "glottis.h"
#include "simplexnoise1234.h"

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

static constexpr float lambda_fn(int i) { return i / static_cast<float>(kFloatsPerDSPVector); }

Glottis::Glottis()
{
	const auto t = 0.7f;

	ui_tenseness_ = 1.0f - std::cos(t * float(M_PI) * 0.5f);
	loudness_ = std::pow(ui_tenseness_, 0.25f);

	setup_waveform(0.0f);
}

ml::DSPVector Glottis::operator()(int SR, const ml::DSPVector& aspirate_noise)
{
	ml::DSPVector out;

	constexpr ml::DSPVector ramp { lambda_fn };

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		out[i] = step(SR, ramp[i], aspirate_noise[i]);
	}

	finish_block();

	return out;
}

float Glottis::step(int SR, float lambda, float noise)
{
	const auto time_step = 1.0f / float(SR);

	time_in_waveform_ += time_step;
	total_time_ += time_step;

	if (time_in_waveform_ > waveform_length_)
	{
		time_in_waveform_ -= waveform_length_;
		setup_waveform(lambda);
	}

	float out = normalized_lf_waveform(time_in_waveform_ / waveform_length_);
	auto aspiration = intensity_ * (1.0f - std::sqrt(ui_tenseness_)) * noise_modulator() * noise;

	aspiration *= 0.2f + 0.02f * snoise1(total_time_ * 1.99f);

	out += aspiration;

	return out;
}

void Glottis::setup_waveform(float lambda)
{
	frequency_ = old_frequency_ * (1.0f - lambda) + new_frequency_ * lambda;

	const auto tenseness = old_tenseness_ * (1.0f - lambda) + new_tenseness_ * lambda;

	rd_ = 3.0f * (1.0f - tenseness);
	waveform_length_ = 1.0f / frequency_;

	const auto rd = std::clamp(rd_, 0.5f, 2.7f);

	const auto ra = -0.01f + 0.048f * rd;
	const auto rk = 0.224f + 0.118f * rd;
	const auto rg = (rk / 4.0f) * (0.5f + 1.2f * rk) / (0.11f * rd - ra * (0.5f + 1.2f * rk));

	const auto ta = ra;
	const auto tp = 1.0f / (2.0f * rg);
	te_ = tp + tp * rk;

	epsilon_ = 1.0f / ta;
	shift_ = std::exp(-epsilon_ * (1.0f - te_));
	delta_ = 1.0f - shift_;

	auto rhs_integral = (1.0f / epsilon_) * (shift_ - 1.0f) + (1.0f - te_) * shift_;

	rhs_integral /= delta_;

	const auto total_lower_integral = -(te_ - tp) / 2.0f + rhs_integral;
	const auto total_upper_integral = -total_lower_integral;

	omega_ = float(M_PI) / tp;
	const auto s = std::sin(omega_ * te_);
	const auto y = -float(M_PI) * s * total_upper_integral / (tp * 2.0f);
	const auto z = std::log(y);
	alpha_ = z / (tp / 2.0f - te_);
	e0_ = -1.0f / (s * std::exp(alpha_ * te_));
}

float Glottis::normalized_lf_waveform(float t)
{
	float out;

	if (t > te_)
	{
		out = (-std::exp(-epsilon_ * (t - te_)) + shift_) / delta_;
	}
	else
	{
		out = e0_ * std::exp(alpha_ * t) * std::sin(omega_ * t);
	}

	return out * intensity_ * loudness_;
}

float Glottis::noise_modulator()
{
	const auto voiced = 0.1f + 0.2f * std::max(0.0f, std::sin(float(M_PI) * 2.0f * time_in_waveform_ / waveform_length_));

	return ui_tenseness_ * intensity_ * voiced + (1.0f - ui_tenseness_ * intensity_) * 0.3f;
}

void Glottis::finish_block()
{
	auto vibrato = 0.0f;

	//vibrato += vibrato_amount * std::sin(2.0f * float(M_PI) * total_time * vibrato_frequency);
	//vibrato += 0.02f * snoise1(total_time * 4.07f);
	//vibrato += 0.04f * snoise1(total_time * 2.15f);

	if (ui_frequency_ > smooth_frequency_)
	{
		smooth_frequency_ = std::min(smooth_frequency_ * 1.1f, ui_frequency_);
	}

	if (ui_frequency_ < smooth_frequency_)
	{
		smooth_frequency_ = std::max(smooth_frequency_ / 1.1f, ui_frequency_);
	}

	old_frequency_ = new_frequency_;
	new_frequency_ = smooth_frequency_ * (1.0f + vibrato);
	old_tenseness_ = new_tenseness_;
	new_tenseness_ = ui_tenseness_ + 0.1f * snoise1(total_time_ * 0.46f) + 0.05f * snoise1(total_time_ * 0.36f);

	// if (!is_touched && always_voice)
	// {
	new_tenseness_ += (3.0f - ui_tenseness_) * (1.0f - intensity_);
	// }

	// if (is_touched || always_voice)
	// {
	intensity_ += 0.13f;
	// }
	// else
	// {
	//	intensity -= 0.05f;
	// }

	intensity_ = std::clamp(intensity_, 0.0f, 1.0f);
}
