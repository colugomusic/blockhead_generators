#include <blink.h>
#include <blink/effect.hpp>
#include <blink/traverser.hpp>

#include "buffer.h"
#include "controller.h"
#include "particle.h"
#include "plugin.h"

class Audio : public blink::Effect
{
public:

	Audio(Freeze* plugin, int instance_group, std::shared_ptr<Freeze::InstanceGroupData> instance_group_data);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:

	float buffer_read(int vector_index, std::size_t row, float pos) const;
	
	Freeze* plugin_;
	std::uint64_t buffer_id_ = 0;
	bool primed_ = false;
	bool record_ = false;
	std::shared_ptr<Freeze::InstanceGroupData> instance_group_data_;
	blink::Traverser block_traverser_;
	Controller controller_;
	Particle particle_;
};