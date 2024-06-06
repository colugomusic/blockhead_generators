#pragma once

#include <blink/dsp.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>
#include "audio_data.h"
#include "model.h"

namespace dsp {

[[nodiscard]] static
auto get_analysis_data(const Model& model, blink_ID sample_id) -> const SampleAnalysis* {
	const auto pos = model.sample_analysis.find(sample_id);
	if (pos == model.sample_analysis.end()) {
		return nullptr;
	}
	return pos->second.get();
}

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data, const blink_WarpPoints& warp_points) -> AudioData {
	AudioData out;
	out.chord.scale = blink::make_chord_data(model.plugin, param_data, model.params.chord.harmonics_scale);
	out.env.amp = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.pan = blink::make_env_data(model.plugin, param_data, model.params.env.pan);
	out.env.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.env.speed = blink::make_env_data(model.plugin, param_data, model.params.env.speed);
	out.env.grain_size = blink::make_env_data(model.plugin, param_data, model.params.env.grain.size);
	out.env.grain_transpose = blink::make_env_data(model.plugin, param_data, model.params.env.grain.transpose);
	out.env.grain_uniformity = blink::make_env_data(model.plugin, param_data, model.params.env.grain.uniformity);
	out.env.harmonics_amount = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.amount);
	out.env.harmonics_spread = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.spread);
	out.env.noise_amount = blink::make_env_data(model.plugin, param_data, model.params.env.noise.amount);
	out.env.noise_color = blink::make_env_data(model.plugin, param_data, model.params.env.noise.color);
	out.slider.amp = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.amp);
	out.slider.pan = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.pan);
	out.slider.pitch = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.pitch);
	out.slider.speed = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.speed);
	out.slider.noise_width = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.noise_width);
	out.slider.sample_offset = blink::make_slider_int_data(model.plugin, param_data, model.params.slider.sample_offset);
	out.option.noise_mode = blink::make_option_data(model.plugin, param_data, model.params.option.noise_mode);
	out.option.reverse_mode = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_mode);
	out.toggle.loop = blink::make_option_data(model.plugin, param_data, model.params.toggle.loop);
	out.toggle.reverse = blink::make_option_data(model.plugin, param_data, model.params.toggle.reverse);
	out.warp_points = &warp_points;
	return out;
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	fudge::init(&unit_dsp->particles[0], 0);
	fudge::init(&unit_dsp->particles[1], 1);
	fudge::init(&unit_dsp->particles[2], 2);
	fudge::init(&unit_dsp->particles[3], 3);
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_SamplerVaryingData& varying, const blink_SamplerUniformData& uniform, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.base.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.base.param_data, *uniform.base.warp_points);
	unit_dsp->block_traverser.generate(uniform.base.id, unit_dsp->block_positions, kFloatsPerDSPVector);
	const auto analysis_data = varying.analysis_ready.value ? get_analysis_data(*model, varying.sample_info->id) : nullptr;
	unit_dsp->controller.process(data, varying, uniform, analysis_data, unit_dsp->block_traverser, unit_dsp->block_positions, uniform.base.data_offset, unit_dsp->SR);
	const auto harmonic_amount = blink::search::vec(data.env.harmonics_amount, unit_dsp->block_positions);
	ml::DSPVectorArray<2> out_vec;
	ml::DSPVector total_amp;
	for (int i = 0; i < 4; i++) {
		auto& particle = unit_dsp->particles[i];
		ml::DSPVector amp; 
		if (i == 0) {
			amp = 1.0f;
		}
		else {
			amp = ml::clamp(harmonic_amount - float(i - 1), ml::DSPVector(0.0f), ml::DSPVector(1.0f));
		} 
		total_amp += amp;
		out_vec += fudge::process(&unit_dsp->particles[i], &unit_dsp->controller, amp);
	}
	out_vec /= ml::repeatRows<2>(total_amp);
	out_vec =
		unit_dsp->noise_gen(
			out_vec,
			float(data.option.noise_mode.value),
			data.env.noise_amount,
			data.env.noise_color,
			blink::search::one(data.slider.noise_width, unit_dsp->block_positions),
			unit_dsp->block_positions);
	out_vec = stereo_pan(out_vec, data.slider.pan.value, data.env.pan, unit_dsp->block_positions);
	const auto amp = blink::search::vec(data.env.amp, unit_dsp->block_positions) * data.slider.amp.value;
	out_vec *= ml::repeatRows<2>(amp);
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

} // dsp