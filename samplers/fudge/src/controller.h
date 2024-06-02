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
		const blink_SamplerBuffer& buffer,
		const blink_SamplerUnitState& unit_state,
		const SampleAnalysis* analysis_data,
		const blink::Traverser& block_traverser,
		const blink::BlockPositions& block_positions,
		int64_t data_offset,
		blink_SR SR);

	const blink::BlockPositions& block_positions() const { return *block_positions_; }
	const std::int32_t data_offset() const { return std::int32_t(data_offset_); }
	const snd::transport::DSPVectorFramePosition& position() const { return sample_positions_; }
	const ml::DSPVectorInt& reset() const { return *resets_; }

	const ml::DSPVector& ff() const { return ff_; }
	const ml::DSPVector& size() const { return size_; }
	const ml::DSPVector& uniformity() const { return uniformity_; }

	const SampleAnalysis* analysis_data() const { return analysis_data_; }
	float frame_increment() const { return frame_increment_; }
	const blink_SampleInfo& sample_info() const { return *(buffer_->sample_info); }
	const blink::SampleData& sample_data() const { return sample_data_; }
	bool sample_loop() const { return sample_loop_; }
	const blink_SamplerBuffer& buffer() const { return *buffer_; }
	float get_harmonic_ratio(int index, int harmonic) const;
	blink_SR SR() const { return SR_; }

private:

	float snap_ratio_to_scale(int index, float ff) const;

	const blink_SamplerBuffer* buffer_;
	const blink::BlockPositions* block_positions_;
	snd::transport::DSPVectorFramePosition sample_positions_;
	const ml::DSPVectorInt* resets_;

	ml::DSPVector ff_;
	ml::DSPVector size_;
	ml::DSPVector uniformity_;
	ml::DSPVector spread_;
	ml::DSPVectorInt scale_;

	blink::SampleData sample_data_;
	const SampleAnalysis* analysis_data_;
	int64_t data_offset_ = 0;
	float frame_increment_ = 1.0f;
	bool sample_loop_;
	blink_SR SR_;
	blink::transform::Stretch stretch_transformer_;
};

}
