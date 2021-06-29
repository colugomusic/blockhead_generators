#include <blink.h>
#include <blink/effect.hpp>

#pragma warning(push, 0)
#pragma warning(pop)

class Lofi;

class Audio : public blink::Effect
{
public:

	Audio(const Lofi* plugin);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	blink_Error reset() override;

private:
	
	const Lofi* plugin_;
};
