#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

static constexpr auto FREQ_GLIDE_TIME_MS = 25.0f;
static constexpr auto MIN_FREQ = 130.0f;
static constexpr auto MAX_FREQ = 4000.0f;

struct AudioData {
	struct {
		blink::uniform::Env frequency;
		blink::uniform::Env resonance;
		blink::uniform::Env spread;
		blink::uniform::Env mix;
	} env;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.frequency = blink::make_env_data(model.plugin, param_data, model.params.env.frequency);
	out.env.resonance = blink::make_env_data(model.plugin, param_data, model.params.env.resonance);
	out.env.spread = blink::make_env_data(model.plugin, param_data, model.params.env.spread);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	auto base_freq  = blink::search::one(data.env.frequency, unit_dsp->block_positions);
	auto spread     = blink::search::one(data.env.spread, unit_dsp->block_positions);
	const auto res  = blink::search::vec(data.env.resonance, unit_dsp->block_positions);
	const auto mix  = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	const auto transform_spread = [](float x) {
		if (x < 0) {
			x = -x;
		}
		return x * x;
	};
	base_freq = blink::math::convert::linear_to_filter_hz(base_freq);
	spread = transform_spread(spread); 
	const auto offset = spread * 1000.0f; 
	std::array<float, 2> freq;
	freq[0] = std::clamp(base_freq - offset, MIN_FREQ, MAX_FREQ);
	freq[1] = std::clamp(base_freq + offset, MIN_FREQ, MAX_FREQ); 
	ml::DSPVectorArray<2> freq_vec; 
	for (int i = 0; i < 2; i++) {
		freq_vec.row(i) = unit_dsp->frequency_glide[i](freq[i]);
	} 
	ml::DSPVectorArray<2> in_vec(in); 
	auto out_vec = unit_dsp->filter->operator()(in_vec, unit_dsp->SR.value, freq_vec, ml::repeatRows<2>(res)); 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector); 
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->filter->clear();
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	const auto glide_samples = static_cast<float>(unit_dsp->SR.value) * (FREQ_GLIDE_TIME_MS / 1000);
	unit_dsp->filter = std::make_unique<snd::audio::filter::Filter_2Pole_AllpassArray<2, 32>>();
	for (auto& glide : unit_dsp->frequency_glide) {
		glide.setGlideTimeInSamples(glide_samples);
	}
}

} // dsp
