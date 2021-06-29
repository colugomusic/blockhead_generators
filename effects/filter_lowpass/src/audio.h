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

	Audio(Filter* plugin, int instance_group);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:
	
	const Filter* plugin_;
	std::array<ml::Lopass, 2> lopass_;
};