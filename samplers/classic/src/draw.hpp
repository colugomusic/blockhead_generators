#pragma once

#include "model.h"
#include <blink/plugin_impl.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>

struct DrawData {
	struct {
		blink::uniform::Env amp;
		blink::uniform::Env pitch;
	} env;
	struct {
		blink::uniform::SliderReal amp;
		blink::uniform::SliderReal pitch;
		blink::uniform::SliderInt sample_offset;
	} slider;
	struct {
		blink::uniform::Option reverse_mode;
	} option;
	struct {
		blink::uniform::Option loop;
		blink::uniform::Option reverse;
	} toggle;
};

[[nodiscard]]
auto make_draw_data(const Model& model, const blink_UniformParamData* param_data) -> DrawData {
	DrawData out;
	out.env.amp              = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.pitch            = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.option.reverse_mode  = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_mode);
	out.slider.amp           = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.amp);
	out.slider.pitch         = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.pitch);
	out.slider.sample_offset = blink::make_slider_int_data(model.plugin, param_data, model.params.slider_int.samp_offs);
	out.toggle.loop          = blink::make_option_data(model.plugin, param_data, model.params.option.loop);
	out.toggle.reverse       = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_toggle);
	return out;
}

auto calculate_amp(const DrawData& data, const blink::BlockPositions& block_positions, float* out) -> void {
	auto amp = blink::search::vec(data.env.amp, block_positions);
	amp *= blink::search::vec(data.slider.amp, block_positions);
	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + block_positions.count, out);
}

[[nodiscard]]
auto draw(Model* model, const blink_SamplerVaryingData& varying, const blink_SamplerUniformData& uniform, blink_FrameCount n, blink_SamplerDrawInfo* out) -> blink_Error {
	const auto data        = make_draw_data(*model, uniform.base.param_data);
	const auto sample_data = blink::SampleData{varying.sample_info, uniform.channel_mode};
	auto frames_remaining = int64_t(n.value);
	int index = 0;
	blink::BlockPositions block_positions;
	while (frames_remaining > 0) {
		auto count = std::min(int(kFloatsPerDSPVector), int(frames_remaining));
		block_positions.add(varying.base.positions + index, count);
		blink::transform::Tape::Config config;
		config.unit_state_id              = uniform.base.id;
		config.env.pitch                  = data.env.pitch.data;
		config.option.reverse             = data.option.reverse_mode.data;
		config.sample_offset              = data.slider.sample_offset.value;
		config.transpose                  = data.slider.pitch.value;
		config.warp_points                = uniform.base.warp_points;
		config.outputs.derivatives.pitch  = true;
		config.outputs.derivatives.warped = true;
		model->draw.tape_transformer.xform(config, block_positions, count);
		auto sculpted_sample_positions = model->draw.tape_transformer.get_pitched_positions().positions / (float(uniform.base.song_rate.value) / sample_data.get_SR().value);
		auto warped_sample_positions = model->draw.tape_transformer.get_warped_positions().positions / (float(uniform.base.song_rate.value) / sample_data.get_SR().value);
		auto final_sample_positions = model->draw.tape_transformer.get_reversed_positions().positions / (float(uniform.base.song_rate.value) / sample_data.get_SR().value);
		if (data.toggle.loop.value) {
			for (int i = 0; i < count; i++) {
				final_sample_positions.set(i, sample_data.get_loop_pos(final_sample_positions[i]));
			}
		} 
		if (data.toggle.reverse.value) {
			final_sample_positions = std::int32_t(sample_data.get_num_frames().value - 1) - final_sample_positions;
		}
		if (out->sculpted_block_positions) {
			const auto doubles = model->draw.tape_transformer.get_pitched_positions().positions.as_doubles();
			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_block_positions + index);
		} 
		if (out->sculpted_sample_positions) {
			const auto doubles = sculpted_sample_positions.as_doubles();
			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_sample_positions + index);
		}
		if (out->warped_block_positions) {
			const auto doubles = model->draw.tape_transformer.get_warped_positions().positions.as_doubles(); 
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
			const auto& pitched_derivatives = model->draw.tape_transformer.get_pitched_derivatives();
			const auto& warped_derivatives = model->draw.tape_transformer.get_warped_derivatives();
			const auto derivatives = pitched_derivatives * warped_derivatives;
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
