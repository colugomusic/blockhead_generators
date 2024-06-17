#pragma once

#include <blink/dsp.hpp>
#include <blink/search.hpp>
#include "model.h"

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env amp;
		blink::uniform::Env pan;
		struct {
			blink::uniform::Env fm;
			blink::uniform::Env pitch;
			blink::uniform::Env waveform;
		} carrier;
		struct {
			blink::uniform::Env fm;
			blink::uniform::Env pitch;
		} modulator;
		struct {
			blink::uniform::Env amount;
			blink::uniform::Env color;
		} noise;
	} env;
	struct {
		blink::uniform::SliderReal amp;
		blink::uniform::SliderReal carrier_pitch;
		blink::uniform::SliderReal noise_width;
		blink::uniform::SliderReal pan;
	} slider;
	struct {
		blink::uniform::Option noise_mode;
	} option;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.amp = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.pan = blink::make_env_data(model.plugin, param_data, model.params.env.pan);
	out.env.carrier.fm = blink::make_env_data(model.plugin, param_data, model.params.env.carrier.fm);
	out.env.carrier.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.carrier.pitch);
	out.env.carrier.waveform = blink::make_env_data(model.plugin, param_data, model.params.env.carrier.waveform);
	out.env.modulator.fm = blink::make_env_data(model.plugin, param_data, model.params.env.modulator.fm);
	out.env.modulator.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.modulator.pitch);
	out.env.noise.amount = blink::make_env_data(model.plugin, param_data, model.params.env.noise.amount);
	out.env.noise.color = blink::make_env_data(model.plugin, param_data, model.params.env.noise.color);
	out.slider.amp = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.amp);
	out.slider.carrier_pitch = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.carrier_pitch);
	out.slider.noise_width = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.noise_width);
	out.slider.pan = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.pan);
	out.option.noise_mode = blink::make_option_data(model.plugin, param_data, model.params.option.noise_mode);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data                = make_audio_data(*model, uniform.param_data);
	auto amp                       = blink::search::vec(data.env.amp, unit_dsp->block_positions);
 	const auto wave                = blink::search::vec(data.env.carrier.waveform, unit_dsp->block_positions);
	const auto env_carrier_pitch   = blink::search::vec(data.env.carrier.pitch, unit_dsp->block_positions);
	const auto env_modulator_pitch = blink::search::vec(data.env.modulator.pitch, unit_dsp->block_positions);
	const auto sld_carrier_pitch   = blink::search::vec(data.slider.carrier_pitch, unit_dsp->block_positions);
	const auto fm0                 = blink::search::vec(data.env.carrier.fm, unit_dsp->block_positions);
	const auto fm1                 = blink::search::vec(data.env.modulator.fm, unit_dsp->block_positions);
	const auto freq0               = blink::math::convert::pitch_to_frequency(60.0f + env_carrier_pitch + sld_carrier_pitch);
	const auto freq1               = blink::math::convert::pitch_to_frequency(60.0f + env_modulator_pitch);
	const auto noise_width         = blink::search::one(data.slider.noise_width, unit_dsp->block_positions);
	amp *= unit_dsp->fade_in(1.0f);
	ml::DSPVector osc_out;
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		const auto fm0_freq = std::max(((fm0[i] * fm0[i] * 20000.0f) * unit_dsp->oscs[1].value) + freq0[i], 0.0f);
		const auto fm1_freq = std::max(((fm1[i] * fm1[i] * 20000.0f) * unit_dsp->oscs[0].value) + freq1[i], 0.0f);
		osc_out[i] = snd::osc::scalar::process(&unit_dsp->oscs[0], fm0_freq / unit_dsp->SR.value, 0.5f, wave[i], snd::osc::scalar::NO_SYNC);
		             snd::osc::scalar::process(&unit_dsp->oscs[1], fm1_freq / unit_dsp->SR.value, 0.5f, snd::osc::scalar::MULTIWAVE_SINE, snd::osc::scalar::NO_SYNC);
	}
	auto out_vec = ml::repeatRows<2>(osc_out);
	out_vec =
		unit_dsp->noise_gen(
			out_vec,
			float(data.option.noise_mode.value),
			data.env.noise.amount,
			data.env.noise.color,
			noise_width,
			unit_dsp->block_positions);
	amp *= data.slider.amp.value;
	out_vec = stereo_pan(out_vec, data.slider.pan.value, data.env.pan, unit_dsp->block_positions);
	out_vec *= ml::repeatRows<2>(amp);
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	snd::osc::scalar::reset(&unit_dsp->oscs[0]);
	snd::osc::scalar::reset(&unit_dsp->oscs[1]);
	unit_dsp->fade_in = {};
	unit_dsp->fade_in.setGlideTimeInSamples(static_cast<float>(unit_dsp->SR.value) * 0.001f);
}

} // dsp