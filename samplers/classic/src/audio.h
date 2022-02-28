#pragma once

#include <blink/sampler_unit.hpp>
#include <blink/sample_data.hpp>
#include <blink/block_positions.hpp>
#include <blink/transform/tape.hpp>
#include "shared/noise_generator.h"

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace classic {

class Plugin;
class Instance;

class Audio : public blink::SamplerUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out) override;
	blink_Error preprocess_sample(void* host, blink_PreprocessCallbacks callbacks) const;

private:

	void reset() override {}

	ml::DSPVectorArray<2> process_stereo_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop);
	ml::DSPVectorArray<2> process_mono_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop);
	
	const Plugin* plugin_;
	blink::transform::Tape tape_transformer_;
	NoiseGenerator noise_gen_;
};

} // classic