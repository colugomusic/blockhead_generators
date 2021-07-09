#include <blink.h>
#include <blink/effect.hpp>
#include "shared/tract.h"
#include "shared/resampler.h"

#pragma warning(push, 0)
#include <DSP/MLDSPBuffer.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace tract {

class Tract;

class Audio : public blink::Effect
{
public:

	Audio(Tract* plugin, int instance_group);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:

	void stream_init() override;

	ml::Bandpass aspirate_filter_;
	ml::Bandpass fricative_filter_;
	ml::NoiseGen noise_;
	const Tract* plugin_;
	ml::DSPBuffer input_buffer_;
	std::array<::Tract, 2> tract_;
	Resampler<2> resampler_;
	Resampler<2> input_resampler_;
};

} // tract