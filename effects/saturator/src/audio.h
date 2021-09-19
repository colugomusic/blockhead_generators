#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/saturator/moron_saturator.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace saturator {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
	snd::audio::saturator::MoronSaturator<2> saturator_;
};

} // saturator
