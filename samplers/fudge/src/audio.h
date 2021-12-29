#pragma once

#include <blink/sampler_unit.hpp>
#include <blink/standard_traversers/fudge.hpp>
#include <blink/sample_data.hpp>
#include <blink/block_positions.hpp>
#include "shared/noise_generator.h"

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

#include "controller.h"
#include "particle.h"

namespace fudge {

class Plugin;
class Instance;

class Audio : public blink::SamplerUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out) override;

private:

	void reset() override {}

	ml::DSPVectorArray<2> add_noise(
		const ml::DSPVectorArray<2>& in,
		int mode,
		const blink_EnvelopeData* env_noise_amount,
		const blink_EnvelopeData* env_noise_color,
		const blink_SliderData* sld_noise_width,
		const blink::BlockPositions& block_positions);
	
	const Plugin* plugin_;
	blink::Traverser block_traverser_;
	blink::TraverserResetter<blink_EnvelopeData> traverser_resetter_;
	NoiseGenerator noise_gen_;
	Controller controller_;
	std::array<Particle, 4> particles_;
};

} // fudge