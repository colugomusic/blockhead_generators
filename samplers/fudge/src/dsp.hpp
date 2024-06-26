#pragma once

#include <blink/dsp.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>
#include <snd/flags.hpp>
#include "audio_data.h"
#include "convert.h"
#include "model.h"

namespace dsp {

[[nodiscard]] static
auto get_analysis_data(const Model& model, blink_ID sample_id) -> const SampleAnalysis* {
	const auto pos = model.sample_analysis.find(sample_id);
	if (pos == model.sample_analysis.end()) {
		return nullptr;
	}
	if (!pos->second->done) {
		return nullptr;
	}
	return pos->second.get();
}

[[nodiscard]] static
auto make_fudge_channel_mode(blink_ChannelMode mode) -> snd::fudge::channel_mode {
	switch (mode) {
		default: case blink_ChannelMode_Stereo:     return snd::fudge::channel_mode::stereo;
		         case blink_ChannelMode_StereoSwap: return snd::fudge::channel_mode::stereo_swap;
		         case blink_ChannelMode_Left:       return snd::fudge::channel_mode::left;
		         case blink_ChannelMode_Right:      return snd::fudge::channel_mode::right;
	}
}

[[nodiscard]] static
auto snap_ratio_to_scale(ml::DSPVectorInt scale, ml::DSPVector ff) -> ml::DSPVector {
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		if (scale[i] == 0) {
			continue;
		}
		ff[i] = snd::convert::FF2P(ff[i]);
		ff[i] = snd::convert::P2FF(snd::snap(ff[i], {uint32_t(scale[i])}));
	}
	return ff;
}

[[nodiscard]] static
auto get_harmonic_ratio(ml::DSPVectorInt scale, ml::DSPVector spread, int harmonic) -> ml::DSPVector {
	return snap_ratio_to_scale(scale, 1.0f + float(harmonic+1) * spread);
}

static
auto process(
	Controller* c,
	const Model& model,
	const AudioData& data,
	const blink_SamplerVaryingData& varying,
	const blink_SamplerUniformData& uniform,
	const blink::Traverser& block_traverser,
	const blink::BlockPositions& block_positions,
	blink_SR SR) -> void
{
	blink::transform::Stretch::Config config;
	config.unit_state_id              = uniform.base.id;
	config.env.speed                  = data.env.speed.data;
	config.option.reverse             = data.option.reverse_mode.data;
	config.sample_offset              = data.slider.sample_offset.value;
	config.speed                      = data.slider.speed.value;
	config.warp_points                = uniform.base.warp_points;
	config.outputs.derivatives.sped   = false;
	config.outputs.derivatives.warped = false;
	c->stretch_transformer(config, block_positions, kFloatsPerDSPVector);
	c->v.sample_position = {c->stretch_transformer.get_reversed_positions().positions};
	c->v.sample_position.value /= float(uniform.base.song_rate.value) / varying.sample_info->SR.value;
	if (data.toggle.reverse.value) {
		c->v.sample_position = {std::int32_t(varying.sample_info->num_frames.value - 1) - c->v.sample_position.value};
	}
	if (data.toggle.loop.value) {
		c->v.sample_position = {blink::math::wrap(c->v.sample_position.value, float(varying.sample_info->num_frames.value))};
	}
	const auto transpose       = blink::search::vec(data.env.grain_transpose, block_positions);
	const auto pitch           = blink::search::vec(data.env.pitch, block_positions) + data.slider.pitch.value;
	const auto size_in_ms      = convert::linear_to_ms(blink::search::vec(data.env.grain_size, block_positions));
	const auto size_in_samples = convert::ms_to_samples(size_in_ms, varying.sample_info->SR.value);
	if (const auto analysis_data = varying.analysis_ready.value ? get_analysis_data(model, varying.sample_info->id) : nullptr) {
		c->v.analysis = analysis_data->analysis.data();
	}
	c->v.frame_increment    = {float(varying.sample_info->SR.value) / SR.value};
	c->v.sample_frame_count = {varying.sample_info->num_frames.value};
	c->v.size               = {blink::math::convert::p_to_ff(blink::math::convert::ff_to_p(size_in_samples) - transpose)};
	c->v.uniformity         = {blink::search::vec(data.env.grain_uniformity, block_positions)};
	c->v.uniformity.value   = c->v.uniformity.value * c->v.uniformity.value * c->v.uniformity.value;
	c->v.channel_count      = {varying.sample_info->num_channels.value};
	c->v.channel_mode       = make_fudge_channel_mode(uniform.channel_mode);
	c->ff                   = blink::math::convert::p_to_ff(pitch);
	c->scale                = {blink::search::vec(data.chord.scale, block_positions)};
	c->spread               = blink::search::vec(data.env.harmonics_spread, block_positions);
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
	unit_dsp->controller.v.SR = {float(unit_dsp->SR.value)};
}

auto process(
	snd::fudge::particle* p, 
	const Controller& c,
	const blink::BlockPositions& block_positions,
	int64_t data_offset, 
	blink::SampleData sample_data,
	const ml::DSPVectorInt& resets) -> ml::DSPVectorArray<2>
{
	auto sample_read_fn = [sample_data](uint8_t channel, float pos) -> float {
		return sample_data.read_frame_interp({channel}, pos);
	};
	ml::DSPVectorArray<2> out;
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		const auto block_pos    = double(block_positions[i]) + data_offset;
		snd::fudge::frame_info f;
		f.idx         = i;
		f.flags       = snd::set_flag(f.flags, f.flags.play, block_pos >= 0);
		f.flags       = snd::set_flag(f.flags, f.flags.reset, resets[i] > 0);
		const auto result = snd::fudge::process(p, c.v, f, sample_read_fn);
		out.row(0)[i] = result.L;
		out.row(1)[i] = result.R;
	} 
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_SamplerVaryingData& varying, const blink_SamplerUniformData& uniform, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.base.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.base.param_data, *uniform.base.warp_points);
	unit_dsp->block_traverser.generate(uniform.base.id, unit_dsp->block_positions, kFloatsPerDSPVector);
	process(&unit_dsp->controller, *model, data, varying, uniform, unit_dsp->block_traverser, unit_dsp->block_positions, unit_dsp->SR);
	const auto harmonic_amount = blink::search::vec(data.env.harmonics_amount, unit_dsp->block_positions);
	ml::DSPVectorArray<2> out_vec;
	ml::DSPVector total_amp;
	const auto sample_data = blink::SampleData(varying.sample_info, uniform.channel_mode);
	for (int i = 0; i < 4; i++) {
		auto& particle = unit_dsp->particles[i];
		if (i == 0) {
			unit_dsp->controller.v.amp            = {1.0f};
			unit_dsp->controller.v.harmonic_ratio = {1.0f};
			unit_dsp->controller.v.ff             = {unit_dsp->controller.ff};
		}
		else {
			unit_dsp->controller.v.harmonic_ratio = {get_harmonic_ratio(unit_dsp->controller.scale, unit_dsp->controller.spread, i)};
			unit_dsp->controller.v.ff             = {unit_dsp->controller.ff * unit_dsp->controller.v.harmonic_ratio.value};
			unit_dsp->controller.v.amp            = {ml::clamp(harmonic_amount - float(i - 1), ml::DSPVector(0.0f), ml::DSPVector(1.0f))};
		} 
		total_amp += unit_dsp->controller.v.amp.value;
		out_vec += process(
			&unit_dsp->particles[i], 
			unit_dsp->controller,
			unit_dsp->block_positions,
			uniform.base.data_offset,
			sample_data,
			unit_dsp->block_traverser.get_resets());
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