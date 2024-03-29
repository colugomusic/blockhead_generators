#include <blink/effect_unit.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace spring_reverb {

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

	ml::LinearGlide glide_size_;
	ml::LinearGlide glide_decay_;

	ml::Allpass<ml::PitchbendableDelay> mAp1, mAp2, mAp3, mAp4;
	ml::Allpass<ml::PitchbendableDelay> mAp5, mAp6, mAp7, mAp8, mAp9, mAp10;
	ml::PitchbendableDelay mDelayL, mDelayR;

	// feedback storage
	ml::DSPVector mvFeedbackL, mvFeedbackR;

	int buffer_count_ = 0;
};

} // spring_reverb