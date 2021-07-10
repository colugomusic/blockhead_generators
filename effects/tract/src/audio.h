#include <blink.h>
#include <blink/effect_unit.hpp>
#include "shared/tract.h"
#include "shared/resampler.h"

#pragma warning(push, 0)
#include <DSP/MLDSPBuffer.h>
#pragma warning(pop)

namespace tract {

static constexpr auto BASE_MODEL_SR = 44100;
static constexpr auto ROWS = 2;

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:

	void stream_init() override;

	const Plugin* plugin_;
	ml::DSPBuffer input_buffer_;
	std::array<::Tract, 2> tract_;
	Resampler<2> resampler_;
	Resampler<2> input_resampler_;
};

} // tract