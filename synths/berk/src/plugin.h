#pragma once

#include <blink/plugin.hpp>

class Berk : public blink::Plugin
{
public:

	static constexpr auto UUID = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	static constexpr auto NAME = "Berk";

	enum class ParameterIndex
	{
		Env_Amp,
		Env_Pan,
		Env_Pitch,
		Env_Buzz,
		Env_Index,
		Env_Diameter,
		Env_TongueIndex,
		Env_TongueDiameter,
		Env_Fricative_Intensity,
		Env_Quality,
	};

	Berk();

	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_pan() const { return *env_pan_; }
	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_buzz() const { return *env_buzz_; }
	const blink::EnvelopeParameter& env_index() const { return *env_index_; }
	const blink::EnvelopeParameter& env_diameter() const { return *env_diameter_; }
	const blink::EnvelopeParameter& env_tongue_index() const { return *env_tongue_index_; }
	const blink::EnvelopeParameter& env_tongue_diameter() const { return *env_tongue_diameter_; }
	const blink::EnvelopeParameter& env_fricative_intensity() const { return *env_fricative_intensity_; }
	const blink::EnvelopeParameter& env_quality() const { return *env_quality_; }

private:

	Params::Envelope env_amp_;
	Params::Envelope env_pan_;
	Params::Envelope env_pitch_;
	Params::Envelope env_buzz_;
	Params::Envelope env_index_;
	Params::Envelope env_diameter_;
	Params::Envelope env_tongue_index_;
	Params::Envelope env_tongue_diameter_;
	Params::Envelope env_fricative_intensity_;
	Params::Envelope env_quality_;
};