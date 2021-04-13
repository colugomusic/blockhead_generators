#include <blink.h>
#include <blink/synth.hpp>
#include <bleat/oscillators/scalar/multi_wave.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Test;

class Audio : public blink::Synth
{
public:

	Audio(const Test* plugin);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	blink_Error reset() override;

private:
	
	const Test* plugin_;
	float prev_pos_;
	std::array<bleat::oscillators::scalar::MultiWaveOsc, 2> oscs_;
};