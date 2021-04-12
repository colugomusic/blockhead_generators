#pragma once

#include <blink/plugin.hpp>

class Test : public blink::Plugin
{
public:

	static constexpr auto UUID = "bb0c455a-dae3-498d-b929-f18a20764528";
	static constexpr auto NAME = "test synth (VERY LOUD BE CAREFUL)";

	enum class ParameterIndex
	{
		Env_Wave,
		Env_Pitch0,
		Env_Pitch1,
		Env_FM0,
		Env_FM1,
	};

	Test();

	const blink::EnvelopeParameter& env_wave() const { return *env_wave_; }
	const blink::EnvelopeParameter& env_p0() const { return *env_p0_; }
	const blink::EnvelopeParameter& env_p1() const { return *env_p1_; }
	const blink::EnvelopeParameter& env_fm0() const { return *env_fm0_; }
	const blink::EnvelopeParameter& env_fm1() const { return *env_fm1_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_wave_;
	std::shared_ptr<blink::EnvelopeParameter> env_p0_;
	std::shared_ptr<blink::EnvelopeParameter> env_p1_;
	std::shared_ptr<blink::EnvelopeParameter> env_fm0_;
	std::shared_ptr<blink::EnvelopeParameter> env_fm1_;
};