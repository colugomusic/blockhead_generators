#include <blink_sampler.h>
#include <blink/sampler.hpp>
#include <blink/standard_traversers/classic.hpp>

class Classic;

class Audio : public blink::Sampler
{
public:

	static constexpr auto REQUIRES_PREPROCESS = false;

	Audio(const Classic* plugin);

	blink_Error process(const blink_SamplerBuffer* buffer, float* out) override;
	blink_Error preprocess_sample(void* host, blink_PreprocessCallbacks callbacks) const;

private:

	const Classic* plugin_;
	blink::Traverser block_traverser_;
	blink::std_traversers::Classic position_traverser_;

};