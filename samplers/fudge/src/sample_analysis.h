#pragma once

#include <atomic>
#include <vector>
#include <blink.h>
#include <snd/audio/autocorrelation.hpp>

struct SampleAnalysis {
	std::vector<snd::poka::result> analysis;
	std::atomic<bool> done = false;
};

bool analyze(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out);