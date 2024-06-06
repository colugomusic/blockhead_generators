#pragma once

#include "model.h"
#include <blink/dsp.hpp>
#include <blink/search.hpp>

namespace dsp {

static inline const std::array<float, 4> PAN_VECTORS = {-0.5f, 0.5f, -1.0f, 1.0f};

struct AudioData {
	struct {
		struct {
			blink::uniform::Chord scale;
		} harmonics;
	} chord;
	struct {
		blink::uniform::Env pitch, feedback, damper, mix;
		struct {
			blink::uniform::Env amount, ratio;
		} fm;
		struct {
			blink::uniform::Env amount, spread, scale_snap_amount, width;
		} harmonics;
	} env;
	struct {
		blink::uniform::SliderReal pitch;
	} slider;
};

struct InputValues {
	struct {
		struct { ml::DSPVectorInt scale; } harmonics;
	} chord;
	struct {
		ml::DSPVector pitch, feedback, damper, mix;
		struct { ml::DSPVector amount, ratio; } fm;
		struct { ml::DSPVector amount, spread, scale_snap_amount, width; } harmonics;
	} env;
	struct {
		float pitch;
	} slider;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.chord.harmonics.scale           = blink::make_chord_data(model.plugin, param_data, model.params.chord.harmonics.scale);
	out.env.damper                      = blink::make_env_data(model.plugin, param_data, model.params.env.damper);
	out.env.feedback                    = blink::make_env_data(model.plugin, param_data, model.params.env.feedback);
	out.env.fm.amount                   = blink::make_env_data(model.plugin, param_data, model.params.env.fm.amount);
	out.env.fm.ratio                    = blink::make_env_data(model.plugin, param_data, model.params.env.fm.ratio);
	out.env.harmonics.amount            = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.amount);
	out.env.harmonics.spread            = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.spread);
	out.env.harmonics.scale_snap_amount = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.scale_snap_amount);
	out.env.harmonics.width             = blink::make_env_data(model.plugin, param_data, model.params.env.harmonics.width);
	out.env.mix                         = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	out.env.pitch                       = blink::make_env_data(model.plugin, param_data, model.params.env.pitch);
	out.slider.pitch                    = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.pitch);
	return out;
}

[[nodiscard]]
auto make_input_values(const Model& model, const blink_UniformParamData* param_data, const blink::BlockPositions& block_positions) -> InputValues {
	InputValues out;
	const auto data = make_audio_data(model, param_data);
	out.chord.harmonics.scale           = blink::search::vec(data.chord.harmonics.scale, block_positions);
	out.env.damper                      = blink::search::vec(data.env.damper, block_positions);
	out.env.feedback                    = blink::search::vec(data.env.feedback, block_positions);
	out.env.fm.amount                   = blink::search::vec(data.env.fm.amount, block_positions);
	out.env.fm.ratio                    = blink::search::vec(data.env.fm.ratio, block_positions);
	out.env.harmonics.amount            = blink::search::vec(data.env.harmonics.amount, block_positions);
	out.env.harmonics.scale_snap_amount = blink::search::vec(data.env.harmonics.scale_snap_amount, block_positions);
	out.env.harmonics.spread            = blink::search::vec(data.env.harmonics.spread, block_positions);
	out.env.harmonics.width             = blink::search::vec(data.env.harmonics.width, block_positions);
	out.env.mix                         = blink::search::vec(data.env.mix, block_positions);
	out.env.pitch                       = blink::search::vec(data.env.pitch, block_positions);
	out.slider.pitch                    = data.slider.pitch.value;
	return out;
}

[[nodiscard]]
auto dampener(Resonator* r, const ml::DSPVectorArray<2>& dry, blink_SR SR, const ml::DSPVectorArray<2>& freq, const ml::DSPVectorArray<2>& mix) -> ml::DSPVectorArray<2> {
	r->filter(dry, int(SR.value), freq);
	return ml::lerp(dry, r->filter.lp(), mix);
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	static constexpr auto EPSILON = 0.0000001f;
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto input_values = make_input_values(*model, uniform.param_data, unit_dsp->block_positions);
	const auto damper_mix = ml::repeatRows<2>(input_values.env.damper);
	const auto damper_freq = ml::repeatRows<2>(blink::math::convert::linear_to_filter_hz<5, 30000>(1.0f - blink::math::ease::quadratic::in(input_values.env.damper)));
	const auto base_pitch = input_values.env.pitch + input_values.slider.pitch + 60.0f;
	const auto base_frequency = blink::math::convert::pitch_to_frequency(base_pitch);
	const auto fm_amount_curve = [](const ml::DSPVector& x) { return x*x*x*x*x*x; };
	const auto fm_amount = fm_amount_curve(input_values.env.fm.amount);
	const auto fm_ratio = convert::linear_to_ratio(input_values.env.fm.ratio);
	const auto fm_freq = base_frequency * fm_ratio;
	const auto fm = unit_dsp->fm_source(fm_freq / unit_dsp->SR_vec);
	const auto pitch = base_pitch + (fm * fm_amount * 60.0f);
	const auto frequency = ml::repeatRows<2>(blink::math::convert::pitch_to_frequency(pitch));
	const auto feedback = blink::math::ease::exponential::out(ml::repeatRows<2>(input_values.env.feedback));
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;
	const auto run_resonator = [unit_dsp](
		Resonator* resonator,
		const ml::DSPVectorArray<2>& freq,
		const ml::DSPVectorArray<2>& feedback,
		const ml::DSPVectorArray<2>& damper_freq,
		const ml::DSPVectorArray<2>& damper_mix,
		const ml::DSPVectorArray<2>& dry)
	{
		const auto dampener = [unit_dsp, resonator, &damper_mix, &damper_freq](const ml::DSPVectorArray<2>& dry) {
			return dsp::dampener(resonator, dry, unit_dsp->SR, damper_freq, damper_mix);
		};
		auto delay_samples = ml::repeatRows<2>(unit_dsp->SR_vec) / freq;
		return resonator->delay(dry, delay_samples, feedback, dampener);
	};
	const auto clamp01 = [](const ml::DSPVector& x) {
		static const ml::DSPVector ZERO{0.0f};
		static const ml::DSPVector ONE{1.0f}; 
		return ml::clamp(x, ZERO, ONE);
	}; 
	const auto pan_harmonic = [clamp01](const ml::DSPVectorArray<2>& dry, const ml::DSPVector& pan) {
		const auto LL = clamp01(ml::DSPVector{1.0f} - pan); // Amount of L to contribute to L
		const auto LR = clamp01(ml::DSPVector{0.0f} - pan); // Amount of R to contribute to L
		const auto RL = clamp01(pan);          // Amount of L to contribute to R
		const auto RR = clamp01(pan + 1.0f);   // Amount of R to contribute to R 
		ml::DSPVectorArray<2> out; 
		out.row(0) = LL * dry.constRow(0) + LR * dry.constRow(1);
		out.row(1) = RL * dry.constRow(0) + RR * dry.constRow(1); 
		return out;
	}; 
	const auto get_harmonic_pan = [clamp01, input_values](int index) {
		return PAN_VECTORS[index] * input_values.env.harmonics.width * clamp01(input_values.env.harmonics.amount);
	};
	out_vec = run_resonator(&unit_dsp->fundamental, frequency, feedback, damper_freq, damper_mix, in_vec);
	out_vec = pan_harmonic(out_vec, get_harmonic_pan(0));
	const auto get_harmonic_ratio = [&input_values](int harmonic) {
		const auto ff = 1.0f + (float(harmonic + 1) * input_values.env.harmonics.spread);
		const auto harmonic_pitch = blink::math::convert::ff_to_p(ff);
		const auto snapped_pitch = blink::snap_pitch_to_scale(harmonic_pitch, input_values.chord.harmonics.scale);
		const auto pitch = ml::lerp(harmonic_pitch, snapped_pitch, input_values.env.harmonics.scale_snap_amount);
		return blink::math::convert::p_to_ff(pitch);
	};
	ml::DSPVectorArray<2> amp { 1.0f };
	for (int i = 0; i < unit_dsp->harmonics.size(); i++) {
		const auto harmonic_amp_v = ml::clamp(input_values.env.harmonics.amount - float(i), {0.0f}, {1.0f});
		if (ml::max(harmonic_amp_v) > EPSILON) {
			auto& filter = unit_dsp->harmonics[i].filter;
			const auto harmonic_amp = ml::repeatRows<2>(harmonic_amp_v);
			const auto ratio = get_harmonic_ratio(i);
			const auto harmonic_frequency = frequency * ml::repeatRows<2>(ratio);
			auto harmonic_out = run_resonator(&unit_dsp->harmonics[i], harmonic_frequency, feedback, damper_freq, damper_mix, in_vec);
			harmonic_out = pan_harmonic(harmonic_out, get_harmonic_pan(i + 1));
			out_vec += harmonic_out * harmonic_amp;
			amp += harmonic_amp;
		}
		else {
			unit_dsp->harmonics[i].delay.clear();
			unit_dsp->harmonics[i].filter.clear();
		}
	} 
	out_vec /= amp; 
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(input_values.env.mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto init(Resonator* r, float SR_f) -> void {
	r->delay.resize(SR_f);
	r->filter = {};
}

auto init(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->SR_f = float(unit_dsp->SR.value);
	init(&unit_dsp->fundamental, unit_dsp->SR_f);
	for (auto& harmonic : unit_dsp->harmonics) {
		init(&harmonic, unit_dsp->SR_f);
	}
	unit_dsp->SR_vec = ml::DSPVector(unit_dsp->SR_f);
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->fm_source.clear();
	unit_dsp->fundamental.delay.clear();
	unit_dsp->fundamental.filter = {};
	for (auto& harmonic: unit_dsp->harmonics) {
		harmonic.delay.clear();
		harmonic.filter = {};
	}
}

} // dsp
