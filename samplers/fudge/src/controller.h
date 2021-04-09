#pragma once

#include <blink_sampler.h>
#include <blink/traverser.hpp>
#include <blink/sample_data.hpp>
#include <blink/standard_traversers/fudge.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

class Fudge;
struct SampleAnalysis;
struct AudioData;

class Controller
{
public:

	Controller(const Fudge* plugin);

	void process(
		const AudioData& data,
		const blink_SamplerBuffer& buffer,
		const SampleAnalysis* analysis_data,
		const blink::Traverser& block_traverser,
		float prev_pos);

	const ml::DSPVector& block_position() const { return *block_positions_; }
	const ml::DSPVector& position() const { return sample_positions_; }
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

private:

	const Fudge* plugin_;
	blink::std_traversers::Fudge position_traverser_;

	const blink_SamplerBuffer* buffer_;
	const ml::DSPVector* block_positions_;
	ml::DSPVector sample_positions_;
	const ml::DSPVectorInt* resets_;

	ml::DSPVector ff_;
	ml::DSPVector size_;
	ml::DSPVector uniformity_;

	blink::SampleData sample_data_;
	const SampleAnalysis* analysis_data_;
	float frame_increment_ = 1.0f;
	bool sample_loop_;
};