#pragma once

#include <blink/plugin.hpp>

#include "gui.h"

class Classic : public blink::Plugin
{
public:

	static constexpr auto UUID = "bd64e4c8-f788-433b-a42a-d375afd92503";
	static constexpr auto NAME = "Classic";

	enum class ParameterIndex
	{
		Env_Amp = 0,
		Env_Pan = 1,
		Env_Pitch = 2,

		Sld_Amp = 3,
		Sld_Pan = 4,
		Sld_Pitch = 5,
		Sld_SampleOffset = 6,

		Tog_Loop = 7,
		Tog_Reverse = 8,
	};

	Classic();

	GUI& gui();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_pan() const { return *env_pan_; }
	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }

	const blink::SliderParameter<float>& slider_pitch() const { return *sld_pitch_; }

private:

	GUI gui_;

	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_pan_;
	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::SliderParameter<float>> sld_amp_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pan_;
	std::shared_ptr<blink::SliderParameter<float>> sld_pitch_;
	std::shared_ptr<blink::SliderParameter<int>> sld_sample_offset_;
	std::shared_ptr<blink::ToggleParameter> tog_loop_;
	std::shared_ptr<blink::ToggleParameter> tog_revers_;
};