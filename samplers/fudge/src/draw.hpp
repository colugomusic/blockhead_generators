#pragma once

#include "model.h"
#include <blink/data.hpp>
#include <blink/plugin_impl.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>

struct DrawData {
	struct {
		blink::EnvData amp;
		blink::EnvData speed;
	} env;
	struct {
		blink::SliderRealData amp;
		blink::SliderRealData speed;
		blink::SliderIntData sample_offset;
	} slider;
	struct {
		blink::OptionData reverse_mode;
	} option;
	struct {
		blink::OptionData loop;
		blink::OptionData reverse;
	} toggle;
	const blink_WarpPoints* warp_points;
};

[[nodiscard]]
auto make_draw_data(const Model& model, const blink_ParamData* param_data, const blink_WarpPoints& warp_points) -> DrawData {
	DrawData data;
	data.env.amp = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	data.env.speed = blink::make_env_data(model.plugin, param_data, model.params.env.speed);
	data.slider.amp = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.amp);
	data.slider.speed = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.speed);
	data.slider.sample_offset = blink::make_slider_int_data(model.plugin, param_data, model.params.slider.sample_offset);
	data.option.reverse_mode = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_mode);
	data.toggle.loop = blink::make_option_data(model.plugin, param_data, model.params.toggle.loop);
	data.toggle.reverse = blink::make_option_data(model.plugin, param_data, model.params.toggle.reverse);
	data.warp_points = &warp_points;
	return data;
}

static
auto calculate_amp(const DrawData& data, const blink::BlockPositions& block_positions, float* out) -> void {
	auto amp = blink::search::vec(data.env.amp, block_positions) * data.slider.amp.value;
	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + block_positions.count, out);
}

[[nodiscard]]
auto draw(Model* model, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	const auto data        = make_draw_data(*model, unit_state.unit.param_data, *unit_state.unit.warp_points);
	const auto sample_data = blink::SampleData{buffer.sample_info, unit_state.channel_mode};
	auto frames_remaining = int64_t(n.value);
	int index = 0;
	blink::BlockPositions block_positions;
	while (frames_remaining > 0) {
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining)); 
		block_positions.add(buffer.unit.positions + index, count); 
		blink::transform::Stretch::Config config; 
		config.unit_state_id = unit_state.unit.id;
		config.env.speed = data.env.speed.data;
		config.option.reverse = data.option.reverse_mode.data;
		config.sample_offset = data.slider.sample_offset.value;
		config.speed = data.slider.speed.value;
		config.warp_points = data.warp_points;
		config.outputs.derivatives.sped = true;
		config.outputs.derivatives.warped = true;
		model->draw.stretch_transformer(config, block_positions, count);
		auto sculpted_sample_positions = model->draw.stretch_transformer.get_sped_positions().positions / (float(buffer.unit.song_rate.value) / sample_data.get_SR().value);
		auto warped_sample_positions = model->draw.stretch_transformer.get_warped_positions().positions / (float(buffer.unit.song_rate.value) / sample_data.get_SR().value);
		auto final_sample_positions = model->draw.stretch_transformer.get_reversed_positions().positions / (float(buffer.unit.song_rate.value) / sample_data.get_SR().value);
		if (data.toggle.loop.value) {
			for (int i = 0; i < count; i++) {
				final_sample_positions.set(i, sample_data.get_loop_pos(final_sample_positions[i]));
			}
		}
		if (data.toggle.reverse.value) {
			final_sample_positions = std::int32_t(sample_data.get_num_frames().value - 1) - final_sample_positions;
		}
		if (out->sculpted_block_positions) {
			const auto doubles = model->draw.stretch_transformer.get_sped_positions().positions.as_doubles(); 
			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_block_positions + index);
		} 
		if (out->sculpted_sample_positions) {
			const auto doubles = sculpted_sample_positions.as_doubles(); 
			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_sample_positions + index);
		} 
		if (out->warped_block_positions) {
			const auto doubles = model->draw.stretch_transformer.get_warped_positions().positions.as_doubles(); 
			std::copy(doubles[0].data(), doubles[0].data() + count, out->warped_block_positions + index);
		} 
		if (out->warped_sample_positions) {
			const auto doubles = warped_sample_positions.as_doubles(); 
			std::copy(doubles[0].data(), doubles[0].data() + count, out->warped_sample_positions + index);
		} 
		if (out->final_sample_positions) {
			const auto doubles = final_sample_positions.as_doubles(); 
			std::copy(doubles[0].data(), doubles[0].data() + count, out->final_sample_positions + index);
		} 
		if (out->waveform_derivatives) {
			const auto& sped_derivatives = model->draw.stretch_transformer.get_sped_derivatives();
			const auto& warped_derivatives = model->draw.stretch_transformer.get_warped_derivatives();
			const auto derivatives = sped_derivatives * warped_derivatives;
			std::copy(derivatives.getConstBuffer(), derivatives.getConstBuffer() + count, out->waveform_derivatives + index);
		} 
		if (out->amp) {
			calculate_amp(data, block_positions, out->amp + index);
		} 
		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}
	return BLINK_OK;
}
