#include <blink.h>
#include <blink/effect.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class SpringReverb;

class Audio : public blink::Effect
{
public:

	Audio(const SpringReverb* plugin);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	blink_Error reset() override;

private:
	
	const SpringReverb* plugin_;
	float prev_pos_;

	ml::LinearGlide glide_size_;
	ml::LinearGlide glide_decay_;

	ml::Allpass<ml::PitchbendableDelay> mAp1, mAp2, mAp3, mAp4;
	ml::Allpass<ml::PitchbendableDelay> mAp5, mAp6, mAp7, mAp8, mAp9, mAp10;
	ml::PitchbendableDelay mDelayL, mDelayR;

	// feedback storage
	ml::DSPVector mvFeedbackL, mvFeedbackR;

	int buffer_count_ = 0;
};