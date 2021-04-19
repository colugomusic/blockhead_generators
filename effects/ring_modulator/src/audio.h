#include <blink.h>
#include <blink/effect.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class RingModulator;

class Audio : public blink::Effect
{
public:

	Audio(const RingModulator* plugin);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	blink_Error reset() override;

private:
	
	const RingModulator* plugin_;
	ml::SineGen sine_;
};