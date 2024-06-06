#pragma once

#include <blink/dsp.hpp>
#include <blink/sample_data.hpp>
#include <blink/search.hpp>
#include <snd/ease.hpp>
#include "model.h"

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env amp;
		blink::uniform::Env pan;
		blink::uniform::Env pitch;
		blink::uniform::Env noise_amount;
		blink::uniform::Env noise_color;
	} env;
	struct {
		blink::uniform::SliderReal amp;
		blink::uniform::SliderReal pan;
		blink::uniform::SliderReal pitch;
		blink::uniform::SliderReal noise_width;
		blink::uniform::SliderInt sample_offset;
	} slider;
	struct {
		blink::uniform::Option noise_mode;
		blink::uniform::Option reverse_mode;
	} option;
	struct {
		blink::uniform::Option loop;
		blink::uniform::Option reverse;
	} toggle;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.amp              = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.pan              = blink::make_env_data(model.plugin, param_data, model.params.env.pan);
	out.env.pitch            = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.env.noise_amount     = blink::make_env_data(model.plugin, param_data, model.params.env.noise_amount);
	out.env.noise_color      = blink::make_env_data(model.plugin, param_data, model.params.env.noise_color);
	out.option.noise_mode    = blink::make_option_data(model.plugin, param_data, model.params.option.noise_mode);
	out.option.reverse_mode  = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_mode);
	out.slider.amp           = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.amp);
	out.slider.noise_width   = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.noise_width);
	out.slider.pan           = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.pan);
	out.slider.pitch         = blink::make_slider_real_data(model.plugin, param_data, model.params.slider_real.pitch);
	out.slider.sample_offset = blink::make_slider_int_data(model.plugin, param_data, model.params.slider_int.samp_offs);
	out.toggle.loop          = blink::make_option_data(model.plugin, param_data, model.params.option.loop);
	out.toggle.reverse       = blink::make_option_data(model.plugin, param_data, model.params.option.reverse_toggle);
	return out;
}

[[nodiscard]]
auto process_stereo_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition &sample_pos, bool loop) -> ml::DSPVectorArray<2> {
	ml::DSPVectorArray<2> out;
	switch (sample_data.get_channel_mode()) {
		default:
		case blink_ChannelMode_Stereo: {
			return sample_data.read_frames_interp<2>(sample_pos, loop);
		} 
		case blink_ChannelMode_StereoSwap: {
			return ml::rotateRows(sample_data.read_frames_interp<2>(sample_pos, loop), 1);
		} 
		case blink_ChannelMode_Left: {
			return ml::repeatRows<2>(sample_data.read_frames_interp({0}, sample_pos, loop));
		} 
		case blink_ChannelMode_Right: {
			return ml::repeatRows<2>(sample_data.read_frames_interp({1}, sample_pos, loop));
		}
	}
}

[[nodiscard]]
auto process_mono_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop) -> ml::DSPVectorArray<2> {
	return ml::repeatRows<2>(sample_data.read_frames_interp({0}, sample_pos, loop));
}

[[nodiscard]]
auto process_sample(
	const AudioData& data,
	const blink_SamplerVaryingData& varying,
	const blink_SamplerUniformData& uniform,
	const blink::SampleData& sample_data,
	snd::transport::DSPVectorFramePosition sample_pos) -> ml::DSPVectorArray<2>
{
	sample_pos /= float(uniform.base.song_rate.value) / varying.sample_info->SR.value;
	if (data.toggle.reverse.value) {
		sample_pos = std::int32_t(varying.sample_info->num_frames.value - 1) - sample_pos;
	} 
	if (varying.sample_info->num_channels.value > 1) {
		return process_stereo_sample(sample_data, sample_pos, data.toggle.loop.value);
	}
	else
	{
		return process_mono_sample(sample_data, sample_pos, data.toggle.loop.value);
	}
}

auto apply_correction_grains(
	ReverseCorrection* reverse_correction,
	const ml::DSPVectorArray<2>& dry,
	const AudioData& data,
	const blink_SamplerVaryingData& varying,
	const blink_SamplerUniformData& uniform,
	const blink::SampleData& sample_data,
	blink_SR SR,
	const blink::transform::CorrectionGrains& grain_info) -> ml::DSPVectorArray<2>
{
	if (!reverse_correction->grain.on && grain_info.count < 1) {
		return dry;
	}
	auto grains_remaining = grain_info.count;
	snd::transport::DSPVectorFramePosition grain_positions;
	ml::DSPVector xfade;
	for (int i = 0; i < BLINK_VECTOR_SIZE; i++) {
		if (reverse_correction->grain.on) {
			// TODO: multiply by scale here???
			reverse_correction->grain.pos += reverse_correction->grain.ff;
			reverse_correction->grain.vpos += reverse_correction->grain.vff;
			grain_positions.set(i, reverse_correction->grain.pos);
			xfade[i] = snd::ease::quadratic::in_out(snd::inverse_lerp(reverse_correction->grain.beg, reverse_correction->grain.vend, reverse_correction->grain.vpos));
			if (reverse_correction->grain.vpos >= reverse_correction->grain.vend) reverse_correction->grain.on = false;
		}
		else {
			xfade[i] = 1.0f;
			if (grains_remaining > 0 && i == grain_info.buffer_index[grain_info.count - grains_remaining]) {
				reverse_correction->grain.on = true;
				const auto beg = i == 0 ? reverse_correction->dry_positions.prev_pos : reverse_correction->dry_positions[i - 1];
				// i hate math
				const auto a = float(uniform.base.song_rate.value) / varying.sample_info->SR.value;
				const auto b = float(varying.sample_info->SR.value) / SR.value;
				reverse_correction->grain.beg = beg;
				reverse_correction->grain.pos = beg;
				reverse_correction->grain.ff = grain_info.ff[i] * a * b;
				reverse_correction->grain.vpos = beg;
				reverse_correction->grain.vend = beg + grain_info.length[i];
				reverse_correction->grain.vff = std::abs(grain_info.ff[i]);
				grain_positions.set(i, reverse_correction->grain.pos);
				xfade[i] = snd::ease::quadratic::in_out(snd::inverse_lerp(reverse_correction->grain.beg, reverse_correction->grain.vend, reverse_correction->grain.vpos));
				grains_remaining--;
			}
		}
	}
	ml::DSPVectorArray<2> out;
	const auto wet = process_sample(data, varying, uniform, sample_data, grain_positions);
	return ml::lerp(wet, dry, ml::repeatRows<2>(xfade));
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_SamplerVaryingData& varying, const blink_SamplerUniformData& uniform, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.base.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.base.param_data);
	const auto generate_correction_grains = uniform.base.smooth_transitions.value && data.option.reverse_mode.data->points.count > 1;
	blink::transform::Tape::Config config;
	config.unit_state_id              = uniform.base.id;
	config.env.pitch                  = data.env.pitch.data;
	config.option.reverse             = data.option.reverse_mode.data;
	config.sample_offset              = data.slider.sample_offset.value;
	config.transpose                  = data.slider.pitch.value;
	config.warp_points                = uniform.base.warp_points;
	config.outputs.derivatives.pitch  = false;
	config.outputs.derivatives.warped = false;
	config.outputs.correction_grains  = generate_correction_grains;
	unit_dsp->tape_transformer.xform(config, unit_dsp->block_positions, BLINK_VECTOR_SIZE);
	auto sample_pos = unit_dsp->tape_transformer.get_reversed_positions().positions;
	if (generate_correction_grains) {
		unit_dsp->reverse_correction.dry_positions.add(sample_pos, BLINK_VECTOR_SIZE);
	}
	blink::SampleData sample_data(varying.sample_info, uniform.channel_mode);
	auto out_vec = process_sample(data, varying, uniform, sample_data, sample_pos);
	if (generate_correction_grains) {
		out_vec = apply_correction_grains(
			&unit_dsp->reverse_correction,
			out_vec, 
			data, 
			varying,
			uniform,
			sample_data, 
			unit_dsp->SR,
			unit_dsp->tape_transformer.get_correction_grains());
	}
	out_vec =
		unit_dsp->noise_gen(
			out_vec,
			float(data.option.noise_mode.value),
			data.env.noise_amount,
			data.env.noise_color,
			blink::search::one(data.slider.noise_width, unit_dsp->block_positions),
			unit_dsp->block_positions);
	auto amp = blink::search::vec(data.env.amp, unit_dsp->block_positions) * data.slider.amp.value;
	out_vec = stereo_pan(out_vec, data.slider.pan.value, data.env.pan, unit_dsp->block_positions);
	out_vec *= ml::repeatRows<2>(amp);
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + BLINK_VECTOR_SIZE);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->reverse_correction.grain = {};
}

} // dsp