#pragma once

#include <blink/plugin.hpp>

class Lofi : public blink::Plugin
{
public:

	static constexpr auto UUID = "90e5db1b-19c0-4ad5-aecf-3622db865584";
	static constexpr auto NAME = "Lofi";

	enum class ParameterIndex
	{
		Env_SR,
		Env_BR,
		Env_Mix,
	};

	Lofi();

	const blink::EnvelopeParameter& env_SR() const { return *env_sr_; }
	const blink::EnvelopeParameter& env_BR() const { return *env_br_; }
	const blink::EnvelopeParameter& env_mix() const { return *env_mix_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_SR_;
	std::shared_ptr<blink::EnvelopeParameter> env_BR_;
	std::shared_ptr<blink::EnvelopeParameter> env_mix_;
};
