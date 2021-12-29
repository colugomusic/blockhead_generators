#include <blink.h>
#include <blink/effect_unit.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace lofi {

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
	float phase_ = 0.0f;
	std::array<float, 2> value_;
};

} // lofi
