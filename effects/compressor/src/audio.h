#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/env_follower.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace compressor {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;

	struct
	{
		snd::audio::EnvFollowerAR<1> env_follower_0;
		snd::audio::EnvFollower<1> env_follower_1;
	} mono_;

	struct
	{
		snd::audio::EnvFollowerAR<2> env_follower_0;
		snd::audio::EnvFollower<2> env_follower_1;
	} stereo_;
};

} // compressor
