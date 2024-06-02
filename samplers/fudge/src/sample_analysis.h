#pragma once

#include <atomic>
#include <vector>
#include <blink.h>
#include "shared/autocorrelation.hpp"

struct SampleAnalysis
{
	std::vector<std::vector<autocorrelation::FrameAnalysis>> data;
	std::atomic<bool> done = false;
};

bool analyze(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out);