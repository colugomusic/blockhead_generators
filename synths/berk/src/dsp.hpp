#pragma once

#include <blink/dsp.hpp>
#include <blink/search.hpp>
#include "model.h"

namespace dsp {

static const ml::DSPVector MIN_THROAT_POSITION(4.0f);
static const ml::DSPVector MAX_THROAT_POSITION(44.0f);
static const ml::DSPVector MIN_THROAT_DIAMETER(-1.0f);
static const ml::DSPVector MAX_THROAT_DIAMETER(3.0f);
static const ml::DSPVector MIN_TONGUE_POSITION(12.0f);
static const ml::DSPVector MAX_TONGUE_POSITION(29.0f);
static const ml::DSPVector MIN_TONGUE_DIAMETER(2.05f);
static const ml::DSPVector MAX_TONGUE_DIAMETER(3.5f);

struct AudioData {
	struct {
		blink::EnvData amp;
		blink::EnvData pan;
		blink::EnvData pitch;
		blink::EnvData buzz;
		blink::EnvData fricative_intensity;
		blink::EnvData quality;
		struct {
			blink::EnvData diameter;
			blink::EnvData position;
		} throat;
		struct {
			blink::EnvData diameter;
			blink::EnvData position;
		} tongue;
	} env;
	 struct {
		blink::SliderRealData amp;
		blink::SliderRealData pan;
		blink::SliderRealData pitch;
	} slider;
};

struct InputValues {
	struct Envelopes {
		ml::DSPVector amp;
		ml::DSPVector pan;
		ml::DSPVector fricative_intensity;
		float pitch;
		float buzz;
		float quality;
		struct {
			ml::DSPVector diameter;
			ml::DSPVector position;
		} throat;
		struct {
			ml::DSPVector diameter;
			ml::DSPVector position;
		} tongue;
	} env;
	struct {
		float amp;
		float pan;
		float pitch;
	} slider;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_ParamData& param_data) -> AudioData {
	AudioData out;
	out.env.amp = blink::make_env_data(model.plugin, &param_data, model.params.env.amp);
	out.env.buzz = blink::make_env_data(model.plugin, &param_data, model.params.env.buzz);
	out.env.fricative_intensity = blink::make_env_data(model.plugin, &param_data, model.params.env.fricative_intensity);
	out.env.pan = blink::make_env_data(model.plugin, &param_data, model.params.env.pan);
	out.env.pitch = blink::make_env_data(model.plugin, &param_data, model.params.env.pitch);
	out.env.quality = blink::make_env_data(model.plugin, &param_data, model.params.env.quality);
	out.env.throat.diameter = blink::make_env_data(model.plugin, &param_data, model.params.env.throat.diameter);
	out.env.throat.position = blink::make_env_data(model.plugin, &param_data, model.params.env.throat.position);
	out.env.tongue.diameter = blink::make_env_data(model.plugin, &param_data, model.params.env.tongue.diameter);
	out.env.tongue.position = blink::make_env_data(model.plugin, &param_data, model.params.env.tongue.position);
	out.slider.amp = blink::make_slider_real_data(model.plugin, &param_data, model.params.slider.amp);
	out.slider.pan = blink::make_slider_real_data(model.plugin, &param_data, model.params.slider.pan);
	out.slider.pitch = blink::make_slider_real_data(model.plugin, &param_data, model.params.slider.pitch);
	return out;
}

[[nodiscard]]
auto make_input_values(const Model& model, const blink_ParamData& param_data, const blink::BlockPositions& positions) -> InputValues {
	const auto audio_data = make_audio_data(model, param_data);
	InputValues input_values;
	input_values.env.amp = blink::search::vec(audio_data.env.amp, positions);
	input_values.env.buzz = blink::search::one(audio_data.env.buzz, positions);
	input_values.env.fricative_intensity = blink::search::vec(audio_data.env.fricative_intensity, positions);
	input_values.env.pan = blink::search::vec(audio_data.env.pan, positions);
	input_values.env.pitch = blink::search::one(audio_data.env.pitch, positions);
	input_values.env.quality = blink::search::one(audio_data.env.quality, positions);
	input_values.env.throat.diameter = blink::search::vec(audio_data.env.throat.diameter, positions);
	input_values.env.throat.position = blink::search::vec(audio_data.env.throat.position, positions);
	input_values.env.tongue.diameter = blink::search::vec(audio_data.env.tongue.diameter, positions);
	input_values.env.tongue.position = blink::search::vec(audio_data.env.tongue.position, positions);
	input_values.slider.amp = blink::search::one(audio_data.slider.amp, positions);
	input_values.slider.pan = blink::search::one(audio_data.slider.pan, positions);
	input_values.slider.pitch = blink::search::one(audio_data.slider.pitch, positions);
	return input_values;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out) -> blink_Error {
	unit_dsp->block_positions.add(buffer.unit.positions, BLINK_VECTOR_SIZE);
	const auto input_values = make_input_values(*model, *unit_state.unit.param_data, unit_dsp->block_positions);
	const auto min_position = ml::min(ml::intToFloat(unit_dsp->block_positions.positions.pos));
	const auto gate = min_position >= -unit_state.unit.data_offset;
	const auto model_SR = int(std::pow(2.0f, input_values.env.quality - 1.0f) * 44100.0f);
	const auto speed = float(model_SR) / unit_dsp->SR.value;
	auto source = [&]() {
		if (!gate) return ml::DSPVector(); 
		const auto noise = unit_dsp->noise();
		unit_dsp->aspirate_filter.mCoeffs = ml::Bandpass::coeffs(500.0f / model_SR, 0.9f);
		unit_dsp->fricative_filter.mCoeffs = ml::Bandpass::coeffs(1000.0f / model_SR, 0.9f);
		const auto fricative_noise = unit_dsp->fricative_filter(noise);
		const auto aspirate_noise = unit_dsp->aspirate_filter(noise); 
		Glottis::Input glottis_input; 
		glottis_input.aspirate_noise = aspirate_noise;
		glottis_input.buzz = blink::math::convert::bi_to_uni(input_values.env.buzz);
		glottis_input.pitch = input_values.env.pitch + input_values.slider.pitch; 
		const auto glottal_output = unit_dsp->glottis(model_SR, speed, glottis_input); 
		Tract::Input tract_input; 
		tract_input.fricatives = true;
		tract_input.diameter = ml::lerp(MIN_THROAT_DIAMETER, MAX_THROAT_DIAMETER, input_values.env.throat.diameter);
		tract_input.fricative_intensity = input_values.env.fricative_intensity;
		tract_input.fricative_noise = fricative_noise * unit_dsp->glottis.noise_modulator();
		tract_input.glottal_output = glottal_output;
		tract_input.index = ml::lerp(MIN_THROAT_POSITION, MAX_THROAT_POSITION, blink::math::convert::bi_to_uni(input_values.env.throat.position));
		tract_input.tongue.diameter = ml::lerp(MIN_TONGUE_DIAMETER, MAX_TONGUE_DIAMETER, input_values.env.tongue.diameter);
		tract_input.tongue.index = ml::lerp(MIN_TONGUE_POSITION, MAX_TONGUE_POSITION, blink::math::convert::bi_to_uni(input_values.env.tongue.position));
		return unit_dsp->tract(model_SR, speed, tract_input);
	};
	auto out_vec = blink::stereo_pan(ml::repeatRows<2>(unit_dsp->resampler(source, speed)), input_values.slider.pan, input_values.env.pan);
	out_vec *= ml::repeatRows<2>(input_values.env.amp) * input_values.slider.amp;
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->glottis.reset();
	unit_dsp->tract.reset();
}

} // dsp