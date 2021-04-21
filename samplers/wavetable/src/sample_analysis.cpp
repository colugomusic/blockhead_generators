#include "sample_analysis.h"

bool analyze(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo& sample_info, SampleAnalysis* out)
{
	constexpr auto ANALYSIS_DEPTH = 128;

	blink_ChannelCount channel = 0;
	float total_progress = 0.0f;

	autocorrelation::AnalysisCallbacks analysis_callbacks;

	analysis_callbacks.get_frames = [host, sample_info, &channel](std::uint32_t index, std::uint32_t n, float* out)
	{
		sample_info.get_data(sample_info.host, channel, index, n, out);
	};

	analysis_callbacks.report_progress = [host, callbacks, sample_info, &total_progress](float progress)
	{
		callbacks.report_progress(host, total_progress + (progress / sample_info.num_channels));
	};

	analysis_callbacks.should_abort = [host, callbacks]()
	{
		return callbacks.should_abort(host);
	};

	out->data.resize(2);
	
	for (; channel < sample_info.num_channels; channel++)
	{
		out->data[channel].resize(sample_info.num_frames);

		if (!autocorrelation::analyze(analysis_callbacks, sample_info.num_frames, ANALYSIS_DEPTH, out->data[channel].data()))
		{
			return false;
		}

		total_progress += 1.0f / sample_info.num_channels;
	}

	out->done = true;

	return true;
}
