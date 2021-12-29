#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/filter/2-pole_allpass_array.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace zap {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const blink_ParameterData* parameter_data, const float* in, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
	snd::audio::filter::Filter_2Pole_AllpassArray<2, 32> filter_;
};

} // zap
