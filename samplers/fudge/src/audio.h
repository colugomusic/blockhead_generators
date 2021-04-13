#include <blink.h>
#include <blink_sampler.h>
#include <blink/sampler.hpp>
#include <blink/standard_traversers/fudge.hpp>
#include <blink/sample_data.hpp>
#include <blink/block_positions.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

#include "controller.h"
#include "particle.h"

class Fudge;

class Audio : public blink::Sampler
{
public:

	static constexpr auto REQUIRES_PREPROCESS = true;

	Audio(const Fudge* plugin);

	blink_Error process(const blink_SamplerBuffer* buffer, float* out) override;

private:

	ml::DSPVectorArray<2> add_noise(
		const ml::DSPVectorArray<2>& in,
		int mode,
		const blink_EnvelopeData* env_noise_amount,
		const blink_EnvelopeData* env_noise_color,
		const blink_SliderData* sld_noise_width,
		const blink::BlockPositions& block_positions);
	
	const Fudge* plugin_;
	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	ml::NoiseGen noise_gen_;
	ml::OnePole noise_filter_;
	Controller controller_;
	std::array<Particle, 4> particles_;
};