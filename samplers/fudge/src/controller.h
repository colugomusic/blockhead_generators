/* TODELETE:
#pragma once

#include <blink.h>
#include <blink/traverser.hpp>
#include <blink/sample_data.hpp>
#include <blink/block_positions.hpp>
#include <blink/transform/stretch.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

struct AudioData;
struct SampleAnalysis;

namespace fudge {

class Controller
{
public:

	void process(
		const AudioData& data,
		const blink_SamplerVaryingData& varying,
		const blink_SamplerUniformData& uniform,
		const SampleAnalysis* analysis_data,
		const blink::Traverser& block_traverser,
		const blink::BlockPositions& block_positions,
		blink_SR SR);

	const snd::transport::DSPVectorFramePosition& position() const { return sample_positions_; }
	const ml::DSPVectorInt& reset() const { return *resets_; }

	const ml::DSPVector& ff() const { return ff_; }
	const ml::DSPVector& size() const { return size_; }
	const ml::DSPVector& uniformity() const { return uniformity_; }

	const SampleAnalysis* analysis_data() const { return analysis_data_; }
	float frame_increment() const { return frame_increment_; }
	const blink::SampleData& sample_data() const { return sample_data_; }
	float get_harmonic_ratio(int index, int harmonic) const;

private:

	float snap_ratio_to_scale(int index, float ff) const;

	snd::transport::DSPVectorFramePosition sample_positions_;
	const ml::DSPVectorInt* resets_;

	ml::DSPVector ff_;
	ml::DSPVector size_;
	ml::DSPVector uniformity_;
	ml::DSPVector spread_;
	ml::DSPVectorInt scale_;

	blink::SampleData sample_data_;
	const SampleAnalysis* analysis_data_;
	float frame_increment_ = 1.0f;
	blink::transform::Stretch stretch_transformer_;
};

}
*/
