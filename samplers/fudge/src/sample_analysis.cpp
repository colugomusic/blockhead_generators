#include "sample_analysis.h"
#include <blink/types.hpp>

auto analyze(void* host, blink_AnalysisCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out) -> blink_AnalysisResult {
	blink_ChannelCount channel = {0};
	float total_progress = 0.0f;
	auto poka_get_frames = [host, sample_info, &channel](size_t index, size_t n, float* out) {
		sample_info.get_data(sample_info.host, channel, {index}, {n}, out);
	};
	auto poka_should_abort = [host, callbacks]() {
		return callbacks.should_abort(host);
	};
	auto poka_report_progress = [host, callbacks, sample_info, &total_progress](float progress) {
		callbacks.report_progress(host, total_progress + (progress / sample_info.num_channels.value));
	};
	auto cbs = snd::poka::make_cbs(poka_get_frames, poka_report_progress, poka_should_abort);
	snd::poka::work poka_work;
	out->analysis.resize(sample_info.num_channels.value); 
	for (; channel.value < sample_info.num_channels.value; channel++) {
		const auto result =
			snd::poka::autocorrelation<snd::poka::mode::classic>(
				&poka_work, cbs, sample_info.num_frames.value, 64, sample_info.SR.value, &out->analysis[channel.value]);
		if (result == snd::poka::result::aborted) {
			return blink_AnalysisResult_Abort;
		}
		total_progress += 1.0f / sample_info.num_channels.value;
	}
	out->done = true;
	return blink_AnalysisResult_OK;
}
