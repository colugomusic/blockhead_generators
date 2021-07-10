#pragma once

#include <blink/synth_plugin.hpp>

namespace test {

class Plugin : public blink::SynthPlugin
{
public:

	static constexpr auto UUID = "bb0c455a-dae3-498d-b929-f18a20764528";
	static constexpr auto NAME = "test synth";

	enum class ParameterIndex
	{
		Env_Amp,
		Env_Wave,
		Env_Pitch0,
		Env_Pitch1,
		Env_FM0,
		Env_FM1,
		Option_NoiseMode,
		Env_NoiseAmount,
		Env_NoiseColor,
		Sld_NoiseWidth,
	};

	Plugin();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_wave() const { return *env_wave_; }
	const blink::EnvelopeParameter& env_p0() const { return *env_p0_; }
	const blink::EnvelopeParameter& env_p1() const { return *env_p1_; }
	const blink::EnvelopeParameter& env_fm0() const { return *env_fm0_; }
	const blink::EnvelopeParameter& env_fm1() const { return *env_fm1_; }
	const blink::EnvelopeParameter& env_noise_amount() const { return *env_noise_amount_; }
	const blink::EnvelopeParameter& env_noise_color() const { return *env_noise_color_; }

private:

	blink::SynthInstance* make_instance() override;

	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_wave_;
	std::shared_ptr<blink::EnvelopeParameter> env_p0_;
	std::shared_ptr<blink::EnvelopeParameter> env_p1_;
	std::shared_ptr<blink::EnvelopeParameter> env_fm0_;
	std::shared_ptr<blink::EnvelopeParameter> env_fm1_;
	std::shared_ptr<blink::OptionParameter> option_noise_mode_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_amount_;
	std::shared_ptr<blink::EnvelopeParameter> env_noise_color_;
	std::shared_ptr<blink::SliderParameter<float>> sld_noise_width_;
};

} // test