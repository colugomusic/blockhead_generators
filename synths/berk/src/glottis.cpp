#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <blink/math.hpp>
#include "glottis.h"
#include "simplexnoise1234.h"

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

static constexpr float lambda_fn(int i) { return i / static_cast<float>(kFloatsPerDSPVector); }

Glottis::Glottis()
{
	reset();
}

void Glottis::reset()
{
	ui_frequency_ = 261.63f;
	smooth_frequency_ = 261.63f;
	vibrato_frequency_ = 6.0f;
	intensity_ = 0.0f;
	time_in_waveform_ = 0.0f;
	total_time_ = 0.0f;
	waveform_length_ = 0.0f;
	frequency_ = 0.0f;
	old_frequency_ = 261.63f;
	new_frequency_ = 261.63f;
	old_tenseness_ = 0.6f;
	new_tenseness_ = 0.6f;
	rd_ = 0.0f;
	alpha_ = 0.0f;
	e0_ = 0.0f;
	epsilon_ = 0.0f;
	shift_ = 0.0f;
	delta_ = 0.0f;
	te_ = 0.0f;
	omega_ = 0.0f;

	setup_waveform(0.0f);
}

static float calculate_tenseness(float pitch, float buzz)
{
	float out = std::clamp((((-pitch) + 24.0f) / 60.0f), 0.0f, 1.0f);

	if (buzz > 0.5f)
	{
		out = blink::math::lerp(out, 1.0f, (buzz - 0.5f) * 2.0f);
	}
	else
	{
		out = blink::math::lerp(0.0f, out, buzz * 2.0f);
	}
	
	return out;
}

ml::DSPVector Glottis::operator()(int SR, float speed, const Input& input)
{
	ml::DSPVector out;

	ui_frequency_ = blink::math::convert::pitch_to_frequency(input.pitch + 60.0f);

	ui_tenseness_ = calculate_tenseness(input.pitch, input.buzz);

	const auto loudness = std::pow(ui_tenseness_, 0.25f);

	constexpr ml::DSPVector ramp { lambda_fn };

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		out[i] = step(1.0f / float(SR), i, ramp[i]);
	}

	float intensity = input.auto_attack ? intensity_ : 1.0f;

	out *= intensity * loudness;

	auto aspiration = intensity * (1.0f - std::sqrt(ui_tenseness_)) * noise_modulator_ * input.aspirate_noise;

	aspiration *= 0.2f + 0.02f * snoise1(total_time_vec_ * 1.99f);

	finish_block(speed, input);

	return out + aspiration;
}

float Glottis::step(float time_step, int i, float lambda)
{
	time_in_waveform_ += time_step;
	total_time_ += time_step;

	if (time_in_waveform_ > waveform_length_)
	{
		time_in_waveform_ -= waveform_length_;
		setup_waveform(lambda);
	}

	noise_modulator_[i] = noise_modulator_step();
	total_time_vec_[i] = total_time_;

	return normalized_lf_waveform(time_in_waveform_ / waveform_length_);
}

void Glottis::setup_waveform(float lambda)
{
	frequency_ = ml::lerp(old_frequency_, new_frequency_, lambda);

	const auto tenseness = ml::lerp(old_tenseness_, new_tenseness_, lambda);

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

	return out;
}

float Glottis::noise_modulator_step()
{
	const auto voiced = 0.1f + 0.2f * std::max(0.0f, std::sin(float(M_PI) * 2.0f * time_in_waveform_ / waveform_length_));

	return ui_tenseness_ * intensity_ * voiced + (1.0f - ui_tenseness_ * intensity_) * 0.3f;
}

void Glottis::finish_block(float speed, const Input& input)
{
	auto vibrato = 0.0f;

	vibrato += vibrato_amount_ * std::sin(2.0f * float(M_PI) * total_time_ * vibrato_frequency_);
	vibrato += wobble_amount_ * 0.02f * snoise1(total_time_ * 4.07f);
	vibrato += wobble_amount_ * 0.04f * snoise1(total_time_ * 2.15f);

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

	new_tenseness_ += ((3.0f - ui_tenseness_) * speed) * (1.0f - intensity_);
	intensity_ = std::clamp(intensity_ + (0.13f * speed), 0.0f, 1.0f);
}
