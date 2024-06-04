#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "particle.h"
#include "controller.h"
#include "sample_analysis.h"
#include <blink/math.hpp>
#include <cassert>

namespace fudge {

[[nodiscard]] static
auto other_grain(Particle* p, int idx) -> Grain& { return p->grains[idx == 0 ? 1 : 0]; }

[[nodiscard]] static
auto adjust_channel_pos(Particle* p, Controller* controller, int index, int channel, float pos) -> float {
	const auto adjust_amount = 1.0f - controller->uniformity()[index]; 
	if (adjust_amount < 0.000001f) return pos; 
	const auto num_frames = controller->sample_info().num_frames; 
	if (pos > num_frames.value) return pos; 
	const auto analysis_data = controller->analysis_data(); 
	if (!analysis_data) return pos;
	if (!analysis_data->done) return pos; 
	const auto& other = other_grain(p, p->flip_flop);
	const auto other_pos = other.pos[channel];
	if (other_pos + other.frame < 0.0f) return pos;
	const auto other_pos_floor = int(std::floor(other_pos + other.frame));
	const auto diff = pos - (other_pos + other.frame);
	const auto abs_diff = std::abs(diff);
	const auto& channel_analysis_data = analysis_data->data[channel];
	const auto other_wavecycle = other_pos_floor >= channel_analysis_data.size() ? channel_analysis_data[channel_analysis_data.size() - 1].estimated_size : channel_analysis_data[other_pos_floor].estimated_size;
	const auto a = int(std::floor(float(abs_diff) / other_wavecycle));
	float adjusted_pos;
	if (diff > 0.0f) {
		adjusted_pos = (other_pos + other.frame) + (a * other_wavecycle);
	}
	else {
		adjusted_pos = (other_pos + other.frame) - (a * other_wavecycle);
	}
	if (adjusted_pos > num_frames.value) {
		adjusted_pos = pos;
	}
	return blink::math::lerp(pos, adjusted_pos, adjust_amount);
}

[[nodiscard]] static
auto get_mono_position(Particle* p, Controller* controller, int index, float pos, bool adjust) -> float {
	if (!adjust) return pos;
	if (pos <= 0.0f) return pos; 
	return adjust_channel_pos(p, controller, index, 0, pos);
}

[[nodiscard]] static
auto get_stereo_positions(Particle* p, Controller* controller, int index, float pos, bool adjust) -> std::array<float, 2> {
	if (!adjust) return { pos, pos };
	if (pos <= 0.0f) return { pos, pos };
	float pos_L;
	float pos_R;
	switch (controller->sample_data().get_channel_mode()) {
		default:
		case blink_ChannelMode_Stereo: {
			pos_L = adjust_channel_pos(p, controller, index, 0, pos);
			pos_R = adjust_channel_pos(p, controller, index, 1, pos);
			break;
		} 
		case blink_ChannelMode_Left: {
			pos_L = adjust_channel_pos(p, controller, index, 0, pos);
			pos_R = adjust_channel_pos(p, controller, index, 0, pos);
			break;
		} 
		case blink_ChannelMode_Right: {
			pos_L = adjust_channel_pos(p, controller, index, 1, pos);
			pos_R = adjust_channel_pos(p, controller, index, 1, pos);
			break;
		}
	} 
	if (std::abs(pos_R - pos_L) < 0.01f * controller->SR().value) {
		pos_R = pos_L;
	} 
	return { pos_L, pos_R };
}

static
auto trigger_next_grain(Particle* p, Controller* controller, int index, bool adjust) -> void {
	constexpr auto MIN_GRAIN_SIZE = 3.0f;
	constexpr auto MAX_WINDOW_SIZE = 4096.0f;
	p->flip_flop = p->flip_flop == 0 ? 1 : 0;
	const auto pos = controller->position()[index];
	const auto fade_in = pos > 0;
	float pos_L;
	float pos_R;
	if (controller->sample_info().num_channels.value > 1) {
		const auto positions = get_stereo_positions(p, controller, index, pos, adjust);
		pos_L = positions[0];
		pos_R = positions[1];
	}
	else {
		pos_L = get_mono_position(p, controller, index, pos, adjust);
		pos_R = pos_L;
	}
	auto ratio = p->harmonic > 0 ? controller->get_harmonic_ratio(index, p->harmonic) : 1.0f;
	auto ff = controller->ff()[index];
	auto size = std::max(MIN_GRAIN_SIZE, controller->size()[index] * ff);
	p->grains[p->flip_flop].on = true;
	p->grains[p->flip_flop].fade_in = fade_in;
	p->grains[p->flip_flop].pos[0] = pos_L;
	p->grains[p->flip_flop].pos[1] = pos_R;
	p->grains[p->flip_flop].ff = controller->frame_increment() * ff * ratio;
	p->grains[p->flip_flop].size = size;
	p->grains[p->flip_flop].window = std::min(MAX_WINDOW_SIZE, std::floor(size / 3));
	p->grains[p->flip_flop].frame = 0.f;
	p->grains[p->flip_flop].frame_amp = fade_in ? 0.f : 1.f;
	p->grains[p->flip_flop].duck = 1.f;
}

static
auto reset(Particle* p, Controller* controller, int index) -> void {
	p->grains[0].on = false;
	p->grains[1].on = false;
	p->trig_primed = false;
	p->trigger_timer = 0.0f;
	trigger_next_grain(p, controller, index, false);
}

[[nodiscard]] static
auto read_mono_frame(const Particle& p, const Controller& controller, const Grain& grain) -> float {
	const auto pos = grain.pos[0] + grain.frame;
	return pos < 0.f ? 0.f : controller.sample_data().read_frame_interp({0}, pos);
}

[[nodiscard]] static
auto read_stereo_frame(const Particle& p, const Controller& controller, const Grain& grain) -> std::array<float, 2> {
	float L;
	float R; 
	const auto& data = controller.sample_data(); 
	switch (controller.sample_data().get_channel_mode()) {
		default:
		case blink_ChannelMode_Stereo: {
			const auto pos_L = grain.pos[0] + grain.frame;
			const auto pos_R = grain.pos[1] + grain.frame; 
			L = pos_L < 0.f ? 0.f : data.read_frame_interp({0}, pos_L);
			R = pos_R < 0.f ? 0.f : data.read_frame_interp({1}, pos_R); 
			break;
		} 
		case blink_ChannelMode_StereoSwap: {
			const auto pos_L = grain.pos[0] + grain.frame;
			const auto pos_R = grain.pos[1] + grain.frame; 
			R = pos_L < 0.f ? 0.f : data.read_frame_interp({0}, pos_L);
			L = pos_R < 0.f ? 0.f : data.read_frame_interp({1}, pos_R); 
			break;
		} 
		case blink_ChannelMode_Left: {
			const auto pos = grain.pos[0] + grain.frame; 
			L = pos < 0.f ? 0.f : data.read_frame_interp({0}, pos);
			R = L; 
			break;
		} 
		case blink_ChannelMode_Right: {
			const auto pos = grain.pos[1] + grain.frame; 
			R = pos < 0.f ? 0.f : data.read_frame_interp({1}, pos);
			L = R; 
			break;
		}
	} 
	return { L, R };
}

auto init(Particle* p, int harmonic) -> void {
	p->harmonic = harmonic;
}

auto process(Particle* p, Controller* controller, const ml::DSPVector& amp) -> ml::DSPVectorArray<2> {
	ml::DSPVector vec_L;
	ml::DSPVector vec_R;
	for (int j = 0; j < kFloatsPerDSPVector; j++) {
		auto block_pos = controller->block_positions().positions[j];
		auto local_block_pos = block_pos + controller->data_offset();
		if (p->trig_primed && local_block_pos >= 0) {
			reset(p, controller, j);
		}
		else {
			if (controller->reset()[j]) {
				if (local_block_pos < 0) {
					p->trig_primed = true;
				}
				else {
					reset(p, controller, j);
				}
			}
			else if(p->trigger_timer >= std::floor(p->grains[p->flip_flop].size / 2)) {
				p->trigger_timer = 0.f;
				trigger_next_grain(p, controller, j, true);
			}
		}
		for (int g = 0; g < 2; g++) {
			float L = 0.0f;
			float R = 0.0f; 
			auto& grain = p->grains[g]; 
			if (!grain.on) continue; 
			if (grain.frame < grain.window) {
				if (grain.fade_in) {
					grain.frame_amp = blink::math::ease::quadratic::in_out(1.f - ((grain.window - grain.frame) / grain.window)); 
					float other_grain_duck = 1.f - grain.frame_amp; 
					other_grain(p, g).duck = other_grain_duck;
				}
			} 
			float self_duck = 1.f; 
			if (grain.frame > grain.size - grain.window) {
				self_duck = blink::math::ease::quadratic::in_out(1.f - ((grain.frame - (grain.size - grain.window)) / grain.window));
			} 
			auto final_duck = std::min(grain.duck, self_duck);
			auto overall_amp = grain.frame_amp * final_duck * amp[j]; 
			if (overall_amp > 0.f) {
				if (controller->sample_info().num_channels.value > 1) {
					const auto LR = read_stereo_frame(*p, *controller, grain); 
					L = LR[0];
					R = LR[1];
				}
				else {
					L = read_mono_frame(*p, *controller, grain);
					R = L;
				} 
				L *= overall_amp;
				R *= overall_amp;
			} 
			grain.frame += grain.ff; 
			if (grain.frame >= grain.size) {
				grain.on = false;
			} 
			vec_L[j] += L;
			vec_R[j] += R;
		} 
		p->trigger_timer += p->grains[p->flip_flop].ff;
	} 
	return ml::concatRows(vec_L, vec_R);
}

} // fudge
