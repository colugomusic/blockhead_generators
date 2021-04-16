#include <blink.h>
#include <blink/effect.hpp>
#include <blink/traverser.hpp>

#include "buffer.h"
#include "controller.h"
#include "particle.h"

#pragma warning(push, 0)

#pragma warning(pop)

class Freeze;

class Audio : public blink::Effect
{
public:

	Audio(const Freeze* plugin);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	blink_Error reset() override;

private:

	float buffer_read(int vector_index, std::size_t row, float pos) const;
	
	const Freeze* plugin_;
	blink::Traverser block_traverser_;
	FreezeBuffer freeze_buffer_;
	Controller controller_;
	Particle particle_;
};