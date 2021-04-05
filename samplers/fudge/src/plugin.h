#pragma once

#include <blink/plugin.hpp>

#include "gui.h"
#include "sample_analysis.h"

class Fudge : public blink::Plugin
{
public:

	static constexpr auto UUID = "795c8dc6-3b81-4397-abac-071bab83b10f";
	static constexpr auto NAME = "Fudge";

	enum class ParameterIndex
	{
		Option_NoiseMode = 0,
		Sld_NoiseWidth = 1,

		Env_Amp = 2,
		Env_Pan = 3,
		Env_Pitch = 4,
		Env_Speed = 5,
		Env_NoiseAmount = 6,
		Env_NoiseColor = 7,

		Sld_Amp = 8,
		Sld_Pan = 9,
		Sld_Pitch = 10,
		Sld_Speed = 11,
		Sld_SampleOffset = 12,

		Tog_Loop = 13,
		Tog_Reverse = 14,
	};

	Fudge();

	GUI& gui();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_pan() const { return *env_pan_; }
	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_noise_amount() const { return *env_noise_amount_; }
	const blink::EnvelopeParameter& env_noise_color() const { return *env_noise_color_; }

	void preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info);

private:

	std::map<blink_ID, SampleAnalysis> sample_analysis_;
	GUI gui_;

	std::shared_ptr<blink::OptionParameter> option_noise_mode_;
	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_pan_;
	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_speed_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_amount_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_color_;
	std::shared_ptr<blink::SliderParameter<float>> sld_amp_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pan_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pitch_;
	std::shared_ptr<blink::SliderParameter<float>> sld_speed_;
	std::shared_ptr<blink::SliderParameter<int>> sld_sample_offset_;
	std::shared_ptr<blink::SliderParameter<float>> sld_noise_width_;
	std::shared_ptr<blink::ToggleParameter> tog_loop_;
	std::shared_ptr<blink::ToggleParameter> tog_revers_;
};