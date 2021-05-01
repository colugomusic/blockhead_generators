#include <blink.h>
#include <blink/effect.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Filter;

class Audio : public blink::Effect
{
public:

	Audio(const Filter* plugin);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	blink_Error reset() override;

private:
	
	const Filter* plugin_;
	std::array<ml::Bandpass, 2> filter_;
};