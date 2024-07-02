#pragma once

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

class Glottis
{

public:

	struct Input
	{
		bool gate = false;
		bool auto_attack = true;
		float pitch = 0.0f;
		float buzz = 0.0f;
		ml::DSPVector aspirate_noise;
	};

	Glottis();

	ml::DSPVector operator()(int SR, float speed, const Input& input);
	void reset();

	const ml::DSPVector& noise_modulator() const { return noise_modulator_; }

private:

	float step(float time_step, int i, float lambda);
	void setup_waveform(float lambda);
	float normalized_lf_waveform(float t);
	float noise_modulator_step();
	void finish_block(float speed, const Input& input);

	ml::DSPVector noise_modulator_;
	ml::DSPVector total_time_vec_;

	float ui_tenseness_ = 0.6f;
	float ui_frequency_ = 261.63f;
	float smooth_frequency_ = 261.63f;
	float vibrato_amount_ = 0.005f;
	float wobble_amount_ = 1.0f;
	float vibrato_frequency_ = 6.0f;
	float intensity_ = 0.0f;
	float time_in_waveform_ = 0.0f;
	float total_time_ = 0.0f;
	float waveform_length_ = 0.0f;
	float frequency_ = 0.0f;
	float old_frequency_ = 140.0f;
	float new_frequency_ = 140.0f;
	float old_tenseness_ = 0.6f;
	float new_tenseness_ = 0.6f;
	float rd_ = 0.0f;
	float alpha_ = 0.0f;
	float e0_ = 0.0f;
	float epsilon_ = 0.0f;
	float shift_ = 0.0f;
	float delta_ = 0.0f;
	float te_ = 0.0f;
	float omega_ = 0.0f;
};