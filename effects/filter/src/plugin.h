#pragma once

#include <blink/plugin.hpp>

class Filter : public blink::Plugin
{
public:

	static constexpr auto UUID = "3bb94b4c-ec38-4557-8666-8fe2e446b83c";
	static constexpr auto NAME = "Filter (ALPHA)";

	enum class ParameterIndex
	{
		Env_Freq,
		Env_Res,
		Env_Mix,
	};

	Filter();

	const blink::EnvelopeParameter& env_freq() const { return *env_freq_; }
	const blink::EnvelopeParameter& env_res() const { return *env_res_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_freq_;
	std::shared_ptr<blink::EnvelopeParameter> env_res_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};