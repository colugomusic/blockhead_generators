#include <blink.h>
#include <blink/effect_unit.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace rattler {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

private:

	auto process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) -> blink_Error override;
	auto reset() -> void override;
	auto stream_init() -> void override;

	std::array<ml::PitchbendableDelay, 2> delays_;
	ml::DSPVectorArray<2> feedback_;
	ml::LinearGlide fade_in_;
	
	const Plugin* plugin_;
};

} // rattler
