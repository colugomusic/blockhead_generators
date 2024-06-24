/* TODELETE:
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "particle.h"
#include "controller.h"
#include "sample_analysis.h"
#include <blink/math.hpp>
#include <cassert>

namespace fudge {


auto init(Particle* p, int harmonic) -> void {
	p->harmonic = harmonic;
}

auto process(Particle* p, const Controller& controller, const playback_positions& positions, const ml::DSPVector& amp) -> ml::DSPVectorArray<2> {
	ml::DSPVector vec_L;
	ml::DSPVector vec_R;
	for (int j = 0; j < kFloatsPerDSPVector; j++) {
		auto local_block_pos = positions.value[j];
		if (p->trig_primed && local_block_pos >= 0) {
			reset(p, controller, j);
		}
		else {
			if (controller.reset()[j]) {
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
				if (controller.sample_info().num_channels.value > 1) {
					const auto LR = read_stereo_frame(*p, controller, grain); 
					L = LR[0];
					R = LR[1];
				}
				else {
					L = read_mono_frame(*p, controller, grain);
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
*/
