#pragma once

#include <vector>
#include <blink_sampler.h>

using SampleAnalysis = std::vector<std::vector<float>>;

extern bool analyze(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out);