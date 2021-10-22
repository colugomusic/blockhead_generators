#include <blink.h>
#include <blink/effect_unit.hpp>

namespace resonator {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:

	std::array<float, 2> velocity_;
	std::array<float, 2> position_;
	
	const Plugin* plugin_;
};

} // resonator
