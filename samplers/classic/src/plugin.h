#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "instance.h"

namespace classic {

class Plugin : public blink::SamplerPlugin
{
public:

	enum class ParameterIndex
	{
		Option_NoiseMode = 0,
		Sld_NoiseWidth = 1,

		Env_Amp = 2,
		Env_Pan = 3,
		Env_Pitch = 4,
		Env_NoiseAmount = 5,
		Env_NoiseColor = 6,

		Sld_Amp = 7,
		Sld_Pan = 8,
		Sld_Pitch = 9,
		Sld_SampleOffset = 10,

		Tog_Loop = 11,
		Tog_Reverse = 12,
	};

	Plugin();

	GUI& gui();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_pan() const { return *env_pan_; }
	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_noise_amount() const { return *env_noise_amount_; }
	const blink::EnvelopeParameter& env_noise_color() const { return *env_noise_color_; }

private:

	blink::SamplerInstance* make_instance() override { return new Instance(this); }

	GUI gui_;

	std::shared_ptr<blink::OptionParameter> option_noise_mode_;
	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_pan_;
	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_amount_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_color_;
	std::shared_ptr<blink::SliderParameter<float>> sld_amp_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pan_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pitch_;
	std::shared_ptr<blink::SliderParameter<int>> sld_sample_offset_;
	std::shared_ptr<blink::SliderParameter<float>> sld_noise_width_;
	std::shared_ptr<blink::ToggleParameter> tog_loop_;
	std::shared_ptr<blink::ToggleParameter> tog_revers_;
};

} // classic