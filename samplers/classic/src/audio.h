#include <blink.h>
#include <blink_sampler.h>
#include <blink/sampler.hpp>
#include <blink/standard_traversers/classic.hpp>
#include <blink/sample_data.hpp>
#include <blink/block_positions.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Classic;

class Audio : public blink::Sampler
{
public:

	static constexpr auto ENABLE_WARP_MARKERS = false;
	static constexpr auto REQUIRES_PREPROCESS = false;

	Audio(const Classic* plugin);

	blink_Error process(const blink_SamplerBuffer* buffer, float* out) override;
	blink_Error preprocess_sample(void* host, blink_PreprocessCallbacks callbacks) const;

private:

	ml::DSPVectorArray<2> process_stereo_sample(const blink::SampleData& sample_data, const ml::DSPVector& sample_pos, bool loop);
	ml::DSPVectorArray<2> process_mono_sample(const blink::SampleData& sample_data, const ml::DSPVector& sample_pos, bool loop);
	ml::DSPVectorArray<2> add_noise(
		const ml::DSPVectorArray<2>& in,
		int mode,
		const blink_EnvelopeData* env_noise_amount,
		const blink_EnvelopeData* env_noise_color,
		const blink_SliderData* sld_noise_width,
		const blink::BlockPositions& block_positions);
	
	const Classic* plugin_;
	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	blink::std_traversers::Classic position_traverser_;
	ml::NoiseGen noise_gen_;
	ml::OnePole noise_filter_;
};