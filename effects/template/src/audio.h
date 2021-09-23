#include <blink.h>
#include <blink/effect_unit.hpp>

namespace template {

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
};

} // template
