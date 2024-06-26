#pragma once

#include <atomic>
#include <vector>
#include <blink.h>
#include <snd/audio/autocorrelation.hpp>

struct SampleAnalysis {
	std::vector<snd::poka::output> analysis;
	std::atomic<bool> done = false;
};

auto analyze(void* host, blink_AnalysisCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out) -> blink_AnalysisResult;