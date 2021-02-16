#include <blkhdgen_sampler.h>
#include <blkhdgen/sampler.hpp>
#include <blkhdgen/standard_traversers/classic.hpp>

class Classic;

class Audio : public blkhdgen::Sampler
{
public:

	static constexpr auto REQUIRES_PREPROCESS = false;

	Audio(const Classic* plugin);

	blkhdgen_Error process(const blkhdgen_SamplerBuffer* buffer, float* out) override;
	blkhdgen_Error preprocess_sample(void* host, blkhdgen_PreprocessCallbacks callbacks) const;

private:

	const Classic* plugin_;
	blkhdgen::Traverser block_traverser_;
	blkhdgen::std_traversers::Classic position_traverser_;

};