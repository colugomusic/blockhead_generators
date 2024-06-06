#pragma once

#include "model.h"
#include <blink/search.hpp>

namespace dsp {

struct LR { float L = 0.0f, R = 0.0f; };

struct ParticleSettings {
	blink::BlockPositions block_positions;
	ml::DSPVector ff;
	ml::DSPVector size;
	const freeze::FreezeBuffer* buffer;
};

struct AudioData {
	struct {
		blink::uniform::Env pitch;
		blink::uniform::Env formant;
		blink::uniform::Env mix;
	} env;
	struct {
		blink::uniform::SliderReal pitch;
	} slider;
};

auto clear(Particle* p) -> void {
	p->grains[0].on = false;
	p->grains[1].on = false; 
	p->first_grain = true;
	p->trigger_timer = 0.0f;
}

auto trigger_next_grain(Particle* p, const ParticleSettings& settings, int index) -> void {
	constexpr auto MAX_WINDOW_SIZE = 4096.0f;
	const auto size             = settings.size[index];
	const auto ff               = settings.ff[index];
	const auto& buffer          = *settings.buffer;
	const auto frames_available = buffer.frames_available();
	if (frames_available < size * 0.5f * ff) {
		return; 
	}
	p->flip_flop = 1 - p->flip_flop;
	p->grains[p->flip_flop].on = true;
	p->grains[p->flip_flop].dry = p->first_grain;
	p->grains[p->flip_flop].ff = ff;
	p->grains[p->flip_flop].size = size;
	p->grains[p->flip_flop].period = size;
	p->grains[p->flip_flop].window = std::min(MAX_WINDOW_SIZE, std::floor(size / 3));
	p->grains[p->flip_flop].frame = 0.f;
	p->grains[p->flip_flop].frame_amp = 0.f;
	p->grains[p->flip_flop].duck = 1.f; 
	p->first_grain = false; 
}

auto reset(Particle* p, const ParticleSettings& settings, int index) -> void {
	p->queue_reset = false;
	clear(p);
	trigger_next_grain(p, settings, index);
}

template <typename ReadFn> [[nodiscard]]
auto read_stereo_frame(ReadFn read_fn, int vector_index, const Grain& grain) -> LR {
	const auto pos = grain.frame * grain.ff; 
	if (pos < 0) {
		return { 0.0f, 0.0f };
	} 
	const auto L = read_fn(vector_index, 0, pos);
	const auto R = read_fn(vector_index, 1, pos); 
	return { L, R };
}

[[nodiscard]]
auto other_grain(Particle* p, int idx) -> Grain& { return p->grains[1 - idx]; }

template <typename ReadFn> [[nodiscard]]
auto process(Particle* p, const ParticleSettings& settings, ReadFn read_fn, int vector_index, float* dry) -> LR {
	LR out; 
	if (p->queue_reset) {
		reset(p, settings, vector_index);
	}
	else {
		const auto half_period = p->grains[p->flip_flop].period / 2; 
		if (p->trigger_timer >= half_period) {
			p->trigger_timer -= half_period;
			trigger_next_grain(p, settings, vector_index);
		}
	} 
	for (int g = 0; g < 2; g++) {
		float L = 0.0f;
		float R = 0.0f; 
		auto& grain = p->grains[g]; 
		if (!grain.on) continue; 
		if (grain.frame < grain.window) {
			grain.frame_amp = blink::math::ease::quadratic::in_out(1.f - ((grain.window - grain.frame) / grain.window)); 
			float other_grain_duck = 1.f - grain.frame_amp; 
			other_grain(p, g).duck = other_grain_duck;
		} 
		float self_duck = 1.f; 
		if (grain.frame > grain.size - grain.window) {
			self_duck = blink::math::ease::quadratic::in_out(1.f - ((grain.frame - (grain.size - grain.window)) / grain.window));
		} 
		auto final_duck = std::min(grain.duck, self_duck);
		auto overall_amp = grain.frame_amp * final_duck; 
		if (overall_amp > 0.f) {
			const auto LR = read_stereo_frame(read_fn, vector_index, grain); 
			L = LR.L * overall_amp;
			R = LR.R * overall_amp;
		} 
		grain.frame++; 
		if (grain.frame >= grain.size) {
			grain.on = false;
		} 
		out.L += L;
		out.R += R; 
		if (grain.dry && grain.frame < grain.window) {
			*dry = 1.0f - overall_amp;
		}
	} 
	if (!p->grains[0].on && !p->grains[1].on) {
		*dry = 1.0f;
	} 
	p->trigger_timer++; 
	return out;
}

[[nodiscard]]
auto buffer_read(const InstanceDSP& instance_dsp, int vector_index, std::size_t row, float pos) -> float {
	int next = int(std::ceil(pos));
	int prev = int(std::floor(pos));
	float x = pos - float(prev);
	const auto next_value = instance_dsp.buffer.at(row, next);
	const auto prev_value = instance_dsp.buffer.at(row, prev);
	return (x * (next_value - prev_value)) + prev_value;
}

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.pitch = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.env.formant = blink::make_env_data(model.plugin, param_data, model.params.env.formant);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	out.slider.pitch = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.pitch);
	return out;
}

[[nodiscard]]
auto make_particle_settings(const AudioData& data, const freeze::FreezeBuffer& buffer, blink_SR SR, const blink::BlockPositions& block_positions) -> ParticleSettings {
	ParticleSettings out;
	out.block_positions = block_positions;
	const auto pitch = blink::search::vec(data.env.pitch, block_positions) + blink::search::vec(data.slider.pitch, block_positions) + 60.0f;
	const auto formant = blink::search::vec(data.env.formant, block_positions);
	out.ff = blink::math::convert::p_to_ff(formant * 24.0f);
	out.size = float(SR.value) / blink::math::convert::p_to_ff(pitch);
	out.buffer = &buffer;
	return out;
}

auto process(Model* model, blink_UnitIdx unit_idx, InstanceDSP* instance_dsp, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	if (!instance_dsp->master_unit) {
		instance_dsp->master_unit = unit_idx;
		unit_dsp->record = false;
	}
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec(in);
	const auto particle_settings = make_particle_settings(data, instance_dsp->buffer, unit_dsp->SR, unit_dsp->block_positions);
	ml::DSPVector dry; 
	auto read_fn = [instance_dsp](int vector_index, std::size_t row, float pos) {
		return buffer_read(*instance_dsp, vector_index, row, pos);
	};
	for (int i = 0; i < kFloatsPerDSPVector; i++) {
		if (*instance_dsp->master_unit == unit_idx) {
			if (!unit_dsp->record) {
				const auto local_block_position = unit_dsp->block_positions.positions[i] + int32_t(uniform.data_offset); 
				if (local_block_position >= 0) {
					unit_dsp->record = true;
					unit_dsp->particle.queue_reset = true;
				}
			}
			if (unit_dsp->record) {
				instance_dsp->buffer.process(in_vec.constRow(0)[i], in_vec.constRow(1)[i]);
			}
		} 
		auto LR = process(&unit_dsp->particle, particle_settings, read_fn, i, &dry[i]); 
		out_vec.row(0)[i] = LR.L;
		out_vec.row(1)[i] = LR.R;
	} 
	out_vec += ml::repeatRows<2>(dry) * in_vec; 
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto instance_reset(Model* model, InstanceDSP* instance_dsp) -> void {
	instance_dsp->buffer.clear();
	instance_dsp->master_unit = std::nullopt;
}

auto unit_reset(Model* model, InstanceDSP* instance_dsp, UnitDSP* unit_dsp) -> void {
	instance_dsp->buffer.clear();
	unit_dsp->particle.queue_reset = true;
}

} // dsp
