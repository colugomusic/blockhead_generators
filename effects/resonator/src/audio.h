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

	blink_Error process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) override;
	void reset() override;
	void stream_init() override;

private:
	
	struct Resonator
	{
		snd::audio::FeedbackDelay<2> delay;
		snd::audio::filter::Filter_1Pole<2> filter;

		ml::DSPVectorArray<2> dampener(const ml::DSPVectorArray<2>& dry, blink_SR SR, const ml::DSPVectorArray<2>& freq, const ml::DSPVectorArray<2>& mix);
	};

	const Plugin* plugin_;
	float SR_f_;
	ml::DSPVector SR_vec_;
	Resonator fundamental_;
	std::array<Resonator, 3> harmonics_;
	ml::SineGen fm_source_;

	static inline const std::array<float, 4> PAN_VECTORS = { -0.5f, 0.5f, -1.0f, 1.0f };
};

} // resonator
