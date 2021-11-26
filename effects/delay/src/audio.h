#include <array>
#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/filter/1-pole.hpp>
#include <snd/audio/feedback_delay.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#pragma warning(pop)


namespace delay {

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
	snd::audio::FeedbackDelay<2> delay_;
	ml::DSPVector SR_vec_;
	snd::audio::filter::Filter_1Pole<2> filter_;
};

} // delay
