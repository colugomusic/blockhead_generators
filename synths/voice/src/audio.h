#pragma once

#define _USE_MATH_DEFINES

#include <blink.h>
#include <blink/synth.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Voice;

class Audio : public blink::Synth
{
public:

	Audio(Voice* plugin, int instance_group);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	void reset() override;

private:

	ml::DSPVector voice(int SR, const ml::DSPVector& aspirate_noise, const ml::DSPVector& fricative_noise);
	float glottis_step(int SR, float lambda, float noise);
	void glottis_setup_waveform(float lambda);
	float glottis_normalized_lf_waveform(float t);
	float glottis_noise_modulator();
	void glottis_finish_block();

	ml::Bandpass aspirate_filter_;
	ml::Bandpass fricative_filter_;
	ml::NoiseGen noise_;
	const Voice* plugin_;

	struct
	{
		float ui_tenseness = 0.6f;
		float ui_frequency = 140.0f;
		float smooth_frequency = 140.0f;
		float vibrato_amount = 0.005f;
		float vibrato_frequency = 6.0f;
		float intensity = 0.0f;
		float loudness = 1.0f;
		float time_in_waveform = 0.0f;
		float total_time = 0.0f;
		float waveform_length = 0.0f;
		float frequency = 0.0f;
		float old_frequency = 140.0f;
		float new_frequency = 140.0f;
		float old_tenseness = 0.6f;
		float new_tenseness = 0.6f;
		float rd = 0.0f;
		float alpha = 0.0f;
		float e0 = 0.0f;
		float epsilon = 0.0f;
		float shift = 0.0f;
		float delta = 0.0f;
		float te = 0.0f;
		float omega = 0.0f;
	} glottis_;
};