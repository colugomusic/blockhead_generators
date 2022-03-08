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

struct AudioData;
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

	ml::DSPVectorArray<2> process_sample(const AudioData& data, const blink_SamplerBuffer& buffer, const blink::SampleData& sample_data, snd::transport::DSPVectorFramePosition sample_pos);
	ml::DSPVectorArray<2> process_stereo_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop);
	ml::DSPVectorArray<2> process_mono_sample(const blink::SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop);
	
	ml::DSPVectorArray<2> apply_correction_grains(
		const ml::DSPVectorArray<2>& dry,
		const AudioData& data,
		const blink_SamplerBuffer& buffer,
		const blink::SampleData& sample_data,
		const blink::transform::CorrectionGrains& grain_info);

	const Plugin* plugin_;
	blink::transform::Tape tape_transformer_;
	blink::BlockPositions dry_positions_;
	snd::transport::DSPVectorFramePosition tmp;
	NoiseGenerator noise_gen_;

	struct CorrectionGrain
	{
		bool on {};
		float beg {};
		float pos {};
		float ff {};
		float vpos {};
		float vend {};
		float vff {};
	} correction_grain_;
};

} // classic