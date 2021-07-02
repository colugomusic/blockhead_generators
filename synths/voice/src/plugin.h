#pragma once

#include <blink/plugin.hpp>

class Voice : public blink::Plugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Voice";

	enum class ParameterIndex
	{
		Env_Amp,
		Env_Formant,
		Env_Index,
		Env_Diameter,
		Env_TongueIndex,
		Env_TongueDiameter,
		Env_Fricative_Intensity,
	};

	Voice();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_formant() const { return *env_formant_; }
	const blink::EnvelopeParameter& env_index() const { return *env_index_; }
	const blink::EnvelopeParameter& env_diameter() const { return *env_diameter_; }
	const blink::EnvelopeParameter& env_tongue_index() const { return *env_tongue_index_; }
	const blink::EnvelopeParameter& env_tongue_diameter() const { return *env_tongue_diameter_; }
	const blink::EnvelopeParameter& env_fricative_intensity() const { return *env_fricative_intensity_; }

private:

	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_formant_;
	std::shared_ptr<blink::EnvelopeParameter> env_index_;
	std::shared_ptr<blink::EnvelopeParameter> env_diameter_;
	std::shared_ptr<blink::EnvelopeParameter> env_tongue_index_;
	std::shared_ptr<blink::EnvelopeParameter> env_tongue_diameter_;
	std::shared_ptr<blink::EnvelopeParameter> env_fricative_intensity_;
};