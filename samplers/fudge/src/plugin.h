#pragma once

#include <blink/sampler_plugin.hpp>

#include "gui.h"
#include "sample_analysis.h"

namespace fudge {

class Plugin : public blink::SamplerPlugin
{
public:

	static constexpr auto UUID = "795c8dc6-3b81-4397-abac-071bab83b10f";
	static constexpr auto NAME = "Fudge";

	enum class ParameterIndex
	{
		Option_NoiseMode,
		Sld_NoiseWidth,

		Env_Amp,
		Env_Pan,
		Env_Pitch,
		Env_Speed,
		Env_GrainSize,
		Env_GrainTranspose,
		Env_Uniformity,
		Chord_Harmonics_Scale,
		Env_Harmonics_Amount,
		Env_Harmonics_Spread,
		Env_NoiseAmount,
		Env_NoiseColor,

		Sld_Amp,
		Sld_Pan,
		Sld_Pitch,
		Sld_Speed,
		Sld_SampleOffset,

		Tog_Loop,
		Tog_Reverse,
	};

	Plugin();

	GUI& gui();

	const blink::ChordParameter& chord_harmonics_scale() const { return *chord_harmonics_scale_; }
	const blink::EnvelopeParameter& env_harmonics_amount() const { return *env_harmonics_amount_; }
	const blink::EnvelopeParameter& env_harmonics_spread() const { return *env_harmonics_spread_; }
	const blink::EnvelopeParameter& env_amp() const { return *env_amp_; }
	const blink::EnvelopeParameter& env_pan() const { return *env_pan_; }
	const blink::EnvelopeParameter& env_pitch() const { return *env_pitch_; }
	const blink::EnvelopeParameter& env_grain_size() const { return *env_grain_size_; }
	const blink::EnvelopeParameter& env_grain_transpose() const { return *env_grain_transpose_; }
	const blink::EnvelopeParameter& env_uniformity() const { return *env_uniformity_; }
	const blink::EnvelopeParameter& env_noise_amount() const { return *env_noise_amount_; }
	const blink::EnvelopeParameter& env_noise_color() const { return *env_noise_color_; }

	const SampleAnalysis* get_analysis_data(blink_ID sample_id) const;

	void preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info);
	void on_sample_deleted(blink_ID id);

private:

	blink::SamplerInstance* make_instance() override;

	std::map<blink_ID, std::shared_ptr<SampleAnalysis>> sample_analysis_;
	GUI gui_;

	std::shared_ptr<blink::ChordParameter> chord_harmonics_scale_;
	std::shared_ptr<blink::OptionParameter> option_noise_mode_;
	std::shared_ptr<blink::EnvelopeParameter> env_harmonics_amount_;
	std::shared_ptr<blink::EnvelopeParameter> env_harmonics_spread_;
	std::shared_ptr<blink::EnvelopeParameter> env_amp_;
	std::shared_ptr<blink::EnvelopeParameter> env_pan_;
	std::shared_ptr<blink::EnvelopeParameter> env_pitch_;
	std::shared_ptr<blink::EnvelopeParameter> env_speed_;
	std::shared_ptr<blink::EnvelopeParameter> env_grain_size_;
	std::shared_ptr<blink::EnvelopeParameter> env_grain_transpose_;
	std::shared_ptr<blink::EnvelopeParameter> env_uniformity_;
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

} // fudge