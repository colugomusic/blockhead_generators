#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env time;
		blink::uniform::Env feedback;
		blink::uniform::Env width;
		blink::uniform::Env dry;
		blink::uniform::Env wet;
	} env;
	struct {
		blink::uniform::SliderReal width;
		blink::uniform::SliderReal dry;
		blink::uniform::SliderReal wet;
	} slider;
};

struct InputValues {
	struct {
		ml::DSPVector time;
		ml::DSPVector feedback;
		ml::DSPVector width;
		ml::DSPVector dry;
		ml::DSPVector wet;
	} env;
	struct {
		float dry;
		float wet;
		float width;
	} slider;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.dry = blink::make_env_data(model.plugin, param_data, model.params.env.dry);
	out.env.feedback = blink::make_env_data(model.plugin, param_data, model.params.env.feedback);
	out.env.time = blink::make_env_data(model.plugin, param_data, model.params.env.time);
	out.env.wet = blink::make_env_data(model.plugin, param_data, model.params.env.wet);
	out.env.width = blink::make_env_data(model.plugin, param_data, model.params.env.width);
	out.slider.dry = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.dry);
	out.slider.wet = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.wet);
	out.slider.width = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.width);
	return out;
}

[[nodiscard]]
auto make_input_values(const Model& model, const blink_UniformParamData* param_data, const blink::BlockPositions& block_positions) -> InputValues {
	InputValues out;
	AudioData data = make_audio_data(model, param_data);
	out.env.dry = blink::search::vec(data.env.dry, block_positions);
	out.env.feedback = blink::search::vec(data.env.feedback, block_positions);
	out.env.time = blink::search::vec(data.env.time, block_positions);
	out.env.wet = blink::search::vec(data.env.wet, block_positions);
	out.env.width = blink::search::vec(data.env.width, block_positions);
	out.slider.dry = data.slider.dry.value;
	out.slider.wet = data.slider.wet.value;
	out.slider.width = data.slider.width.value;
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto input_values = make_input_values(*model, uniform.param_data, unit_dsp->block_positions);
	const auto dry      = input_values.env.dry * input_values.slider.dry;
	const auto wet      = input_values.env.wet * input_values.slider.wet;
	const auto feedback = input_values.env.feedback;
	auto width = input_values.env.width + input_values.slider.width;
	width = width * ml::abs(width);
	auto time_ms = ml::repeatRows<2>(blink::math::convert::linear_to_delay_time_ms(input_values.env.time));
	time_ms.row(0) += width;
	time_ms.row(1) -= width;
	const auto time_samples{time_ms / 1000 * float(unit_dsp->SR.value)};
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec{};
	auto delay_input = (in_vec * ml::repeatRows<2>(wet) * ml::repeatRows<2>(unit_dsp->fade_in(1.0f))) + unit_dsp->feedback;
	for (auto channel{0}; channel < 2; channel++) {
		out_vec.row(channel) = unit_dsp->delays[channel](delay_input.constRow(channel), time_samples.constRow(channel));
	}
	unit_dsp->feedback = out_vec * ml::repeatRows<2>(feedback);
	out_vec += ml::repeatRows<2>(dry) * in_vec;
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->feedback = {};
	unit_dsp->delays[0].clear();
	unit_dsp->delays[1].clear();
	unit_dsp->fade_in.setValue(0.0f);
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->delays[0].setMaxDelayInSamples(2.0f * unit_dsp->SR.value);
	unit_dsp->delays[1].setMaxDelayInSamples(2.0f * unit_dsp->SR.value);
	unit_dsp->fade_in.setGlideTimeInSamples(0.1f * unit_dsp->SR.value);
	reset(model, unit_dsp);
}

} // dsp
