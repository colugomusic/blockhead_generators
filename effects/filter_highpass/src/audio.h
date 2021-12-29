#include <blink.h>
#include <blink/effect_unit.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace highpass {

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
	std::array<ml::Hipass, 2> filter_;
};

} // highpass