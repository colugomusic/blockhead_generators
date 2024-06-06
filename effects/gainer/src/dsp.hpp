#pragma once

#include "model.h"
#include <blink/dsp.hpp>
#include <blink/search.hpp>

namespace dsp {

struct AudioData {
	struct {
		blink::uniform::Env amp;
		blink::uniform::Env pan;
	} env;
	struct {
		blink::uniform::SliderReal amp;
		blink::uniform::SliderReal pan;
	} slider;
};

[[nodiscard]]
auto make_audio_data(const Model& model, const blink_UniformParamData* param_data) -> AudioData {
	AudioData out;
	out.env.amp = blink::make_env_data(model.plugin, param_data, model.params.env.amp);
	out.env.pan = blink::make_env_data(model.plugin, param_data, model.params.env.pan);
	out.slider.amp = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.amp);
	out.slider.pan = blink::make_slider_real_data(model.plugin, param_data, model.params.slider.pan);
	return out;
}

auto process(Model* model, UnitDSP* unit_dsp, const blink_VaryingData& varying, const blink_UniformData& uniform, const float* in, float* out) -> blink_Error {
	unit_dsp->block_positions.add(varying.positions, BLINK_VECTOR_SIZE);
	const auto data = make_audio_data(*model, uniform.param_data);
	const ml::DSPVectorArray<2> in_vec(in);
	auto out_vec = blink::stereo_pan(in_vec, data.slider.pan.value, blink::search::vec(data.env.pan, unit_dsp->block_positions));
	out_vec *= ml::repeatRows<2>(blink::search::vec(data.env.amp, unit_dsp->block_positions)) * data.slider.amp.value;
	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);
	return BLINK_OK;
}

} // dsp
