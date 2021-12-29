#include <blink.h>
#include <blink/effect_unit.hpp>
#include <blink/traverser.hpp>

#include "buffer.h"
#include "controller.h"
#include "particle.h"
#include "plugin.h"

namespace freeze {

class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const blink_ParameterData* parameter_data, const float* in, float* out) override;
	void reset() override;

private:

	float buffer_read(int vector_index, std::size_t row, float pos) const;
	
	Plugin* plugin_;
	Instance* instance_;
	std::uint64_t buffer_id_ = 0;
	bool primed_ = false;
	bool record_ = false;
	blink::Traverser block_traverser_;
	Controller controller_;
	Particle particle_;
};

}