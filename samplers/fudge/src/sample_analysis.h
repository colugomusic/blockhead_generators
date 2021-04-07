#pragma once

#include <atomic>
#include <vector>
#include <blink_sampler.h>

struct SampleAnalysis
{
	std::vector<std::vector<float>> data;
	std::atomic<bool> done = false;
};

extern bool analyze(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out);