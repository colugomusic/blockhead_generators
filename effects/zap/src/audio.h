#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/filter/2-pole_allpass_array.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace zap {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit {
public:
	Audio(Instance* instance);
	auto process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) -> blink_Error final;
	auto reset() -> void final;
	auto stream_init() -> void final;
private:
	const Plugin* plugin_;
	snd::audio::filter::Filter_2Pole_AllpassArray<2, 32> filter_;
	std::array<ml::LinearGlide, 2> frequency_glide_;
};

} // zap
