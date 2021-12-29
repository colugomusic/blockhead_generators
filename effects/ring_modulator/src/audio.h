#pragma once

#include <blink/effect_unit.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace ring_modulator {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
	ml::SineGen sine_;
};

} // ring_modulator