#include <blink.h>
#include <blink/effect_unit.hpp>

#include <snd/audio/feedback_delay.hpp>
#include <snd/audio/filter/1-pole.hpp>

#pragma warning(push, 0)

#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace resonator {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;
	void stream_init() override;

private:
	
	const Plugin* plugin_;
	float SR_f_;
	ml::DSPVector SR_vec_;
	snd::audio::FeedbackDelay<2> delay_;
	std::array<snd::audio::FeedbackDelay<2>, 3> harmonics_;
	snd::audio::filter::Filter_1Pole<2> filter_;
	ml::SineGen sine_;
};

} // resonator
