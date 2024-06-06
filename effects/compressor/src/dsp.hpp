#pragma once

#include "convert.h"
#include "model.h"
#include <blink/search.hpp>
#include <DSP/MLDSPFunctional.h>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env attack;
		blink::uniform::Env release;
		blink::uniform::Env threshold;
		blink::uniform::Env ratio;
		blink::uniform::Env knee;
		blink::uniform::Env mix;
	} env;
	struct {
		blink::uniform::Option stereo;
	} option;
};

template <size_t ROWS> static
auto linear_to_db(ml::DSPVectorArray<ROWS> x) -> ml::DSPVectorArray<ROWS> {
	return ml::map([](float v) { return blink::math::convert::linear_to_db(v); }, x);
}

template <size_t ROWS> static
auto linear_to_ratio(ml::DSPVectorArray<ROWS> x) -> ml::DSPVectorArray<ROWS> {
	return ml::map([](float v) { return blink::math::convert::linear_to_ratio(v); }, x);
}

template <size_t ROWS> static
auto db_to_linear(ml::DSPVectorArray<ROWS> x) -> ml::DSPVectorArray<ROWS> {
	return ml::map([](float v) { return blink::math::convert::db_to_linear(v); }, x);
}

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.attack = blink::make_env_data(model.plugin, param_data, model.params.env.attack);
	out.env.release = blink::make_env_data(model.plugin, param_data, model.params.env.release);
	out.env.threshold = blink::make_env_data(model.plugin, param_data, model.params.env.threshold);
	out.env.ratio = blink::make_env_data(model.plugin, param_data, model.params.env.ratio);
	out.env.knee = blink::make_env_data(model.plugin, param_data, model.params.env.knee);
	out.env.mix = blink::make_env_data(model.plugin, param_data, model.params.env.mix);
	out.option.stereo = blink::make_option_data(model.plugin, param_data, model.params.option.stereo);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data                = make_audio_data(*model, uniform.param_data);
	const auto attack = convert::linear_to_attack(blink::search::one(data.env.attack, unit_dsp->block_positions)) / 1000;
	const auto release = convert::linear_to_release(blink::search::one(data.env.release, unit_dsp->block_positions)) / 1000;
	const auto threshold = linear_to_db(blink::search::vec(data.env.threshold, unit_dsp->block_positions));
	const auto ratio = linear_to_ratio(blink::search::vec(data.env.ratio, unit_dsp->block_positions));
	const auto knee = blink::search::vec(data.env.knee, unit_dsp->block_positions);
	const auto mix = blink::search::vec(data.env.mix, unit_dsp->block_positions);
	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;
	static const auto EPSILON = ml::DSPVector(0.0000001f);
	static const auto ONE = ml::DSPVector(1.0f);
	ml::DSPVectorArray<2> reduction;
	if (!data.option.stereo.value) { // mono
		unit_dsp->mono.env_follower_0.set_SR(unit_dsp->SR.value);
		unit_dsp->mono.env_follower_0.set_release(release);
		unit_dsp->mono.env_follower_1.set_SR(unit_dsp->SR.value);
		unit_dsp->mono.env_follower_1.set_time(attack);
		const auto peak = unit_dsp->mono.env_follower_1(unit_dsp->mono.env_follower_0((in_vec.constRow(0) + in_vec.constRow(1)) * 0.5f));
		const auto peak_db = linear_to_db(ml::clamp(ml::abs(peak), EPSILON, ONE));
		const auto pre_knee = peak_db - threshold;
		const auto post_knee = (ml::sqrt((pre_knee * pre_knee) + (knee * knee)) + pre_knee) * 0.5f; 
		const auto r = (1.0f - (1.0f / ratio));
		const auto db_reduction = r * post_knee;
		const auto makeup_db = r * ((0.0f - threshold) / 2.0f); 
		reduction = ml::repeatRows<2>(db_to_linear(makeup_db - db_reduction));
	}
	else { // stereo
		unit_dsp->stereo.env_follower_0.set_SR(unit_dsp->SR.value);
		unit_dsp->stereo.env_follower_0.set_release(release);
		unit_dsp->stereo.env_follower_1.set_SR(unit_dsp->SR.value);
		unit_dsp->stereo.env_follower_1.set_time(attack); 
		const auto peak = unit_dsp->stereo.env_follower_1(unit_dsp->stereo.env_follower_0(in_vec));
		const auto peak_db = linear_to_db(ml::clamp(ml::abs(peak), ml::repeatRows<2>(EPSILON), ml::repeatRows<2>(ONE))); 
		const auto knee2 = ml::repeatRows<2>(knee);
		const auto threshold2 = ml::repeatRows<2>(threshold);
		const auto pre_knee = peak_db - threshold2;
		const auto post_knee = (ml::sqrt((pre_knee * pre_knee) + (knee2 * knee2)) + pre_knee) * 0.5f; 
		const auto r = (1.0f - (1.0f / ml::repeatRows<2>(ratio)));
		const auto db_reduction = r * post_knee;
		const auto makeup_db = r * ((0.0f - threshold2) / 2.0f); 
		reduction = db_to_linear(makeup_db - db_reduction);
	} 
	out_vec = in_vec * reduction;
	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix)); 
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

auto reset(Model* model, UnitDSP* unit_dsp) -> void {
	unit_dsp->mono.env_follower_0.reset();
	unit_dsp->mono.env_follower_1.reset();
	unit_dsp->stereo.env_follower_0.reset();
	unit_dsp->stereo.env_follower_1.reset();
}

} // dsp
