#pragma once

#include <cassert>
#include <deque>
#include <functional>
#include <blink/math.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#pragma warning(pop)

namespace autocorrelation {

struct AnalysisCallbacks
{
	std::function<void(std::uint64_t index, std::uint32_t n, float* out)> get_frames;
	std::function<bool()> should_abort;
	std::function<void(float)> report_progress;
};

struct FrameAnalysis
{
	float estimated_size = 0.0f;
	std::uint64_t prev_crossing = 0;
	std::uint64_t next_crossing = 0;
};

// specialized autocorrelation which only considers the distances between zero crossings in the analysis.
// returns false if aborted before the analysis could complete
// allocates memory
inline bool analyze(AnalysisCallbacks callbacks, std::uint64_t n, std::uint32_t depth, FrameAnalysis* out)
{
	if (depth < 4) depth = 4;

	struct ZX
	{
		bool init = false;
		bool up = false;
		float best_peak = 0.0f;
		std::uint64_t best_peak_pos = 0;
		std::uint64_t latest = 0;
	} zx;

	struct Crossing
	{
		std::uint64_t index;
		std::uint64_t distance;
		std::uint64_t peak_index;
		std::uint64_t peak_distance;
		float peak_value;
		bool up;
	};

	std::deque<Crossing> crossings;

	const auto find_best_crossing = [](const std::deque<Crossing>& crossings)
	{
		constexpr auto AUTO_WIN = 0.05f;
		constexpr auto MAX_DIFF = 100.0f;

		auto best = crossings.front().index;
		auto best_diff = MAX_DIFF;
		int depth = 2;

		for (int depth = 2; depth * 2 <= crossings.size(); depth += 2)
		{
			auto a = crossings.begin();
			auto b = crossings.begin() + depth;

			const auto last = b + (depth - 1);

			const auto a_beg = b->index;
			const auto b_beg = (last->index - last->distance);

			const auto a_length = a->index - a_beg;
			const auto b_length = b->index - b_beg;

			auto total_diff = 0.0f;
			auto terrible = false;

			for (int i = 0; i < depth; i++, a++, b++)
			{
				assert(a->up == b->up);

				const auto a_distance = (float(a->index) - a_beg) / a_length;
				const auto b_distance = (float(b->index) - b_beg) / b_length;
				const auto a_peak_distance = (float(a->peak_index) - a_beg) / a_length;
				const auto b_peak_distance = (float(b->peak_index) - b_beg) / b_length;

				float part_diff = 0.0f;

				part_diff += std::abs(a_distance - b_distance);
				part_diff += std::abs(a_peak_distance - b_peak_distance);
				part_diff += std::abs(a->peak_value - b->peak_value);
				part_diff /= (depth / 2);

				total_diff += part_diff;

				if (total_diff >= best_diff)
				{
					terrible = true;
					break;
				}
			}

			if (terrible) continue;

			if (total_diff <= AUTO_WIN * depth * 2)
			{
				return (crossings.begin() + depth)->index;
			}

			if (total_diff < best_diff)
			{
				best_diff = total_diff;
				best = (crossings.begin() + depth)->index;
			}
		}

		return best;
	};

	std::uint64_t best_crossing = 0;
	auto prev_size = 0.0f;

	ml::DSPVector chunk_frames;
	ml::OnePole filter;
	ml::DCBlocker dc_blocker;

	filter.mCoeffs = ml::OnePole::coeffs(0.0011f);
	dc_blocker.mCoeffs = ml::DCBlocker::coeffs(0.011f);

	std::uint64_t index = 0;
	auto frames_remaining = n;
	auto filled_start_frames = false;

	while (frames_remaining > 0)
	{
		if (callbacks.should_abort()) return false;

		auto num_chunk_frames = std::uint32_t(std::min(std::uint64_t(kFloatsPerDSPVector), frames_remaining));

		callbacks.get_frames(index, num_chunk_frames, chunk_frames.getBuffer());

		const auto filtered_frames = filter(dc_blocker(chunk_frames));

		for (std::uint32_t i = 0; i < num_chunk_frames; i++)
		{
			const auto value = filtered_frames[i];

			bool crossed = false;

			if (value > 0.0f && (!zx.init || !zx.up))
			{
				zx.init = true;
				zx.up = true;
				crossed = true;
			}
			else if (value < 0.0f && (!zx.init || zx.up))
			{
				zx.init = true;
				zx.up = false;
				crossed = true;
			}
			else
			{
				if (zx.up && value > zx.best_peak)
				{
					zx.best_peak = value;
					zx.best_peak_pos = (index + i);
				}
				else if (!zx.up && value < zx.best_peak)
				{
					zx.best_peak = value;
					zx.best_peak_pos = (index + i);
				}
			}

			if (crossed)
			{
				Crossing crossing;

				crossing.index = (index + i);
				crossing.distance = crossing.index - zx.latest;
				crossing.peak_index = zx.best_peak_pos;
				crossing.peak_distance = zx.best_peak_pos - zx.latest;
				crossing.peak_value = zx.best_peak;
				crossing.up = zx.up;

				crossings.push_front(crossing);

				if (crossings.size() > depth)
				{
					crossings.pop_back();
				}

				best_crossing = find_best_crossing(crossings);

				const auto found = best_crossing != (index + i);
				const auto size = !found ? prev_size : float(crossing.index - best_crossing);

				if (found && !filled_start_frames)
				{
					for (std::uint32_t j = 0; j < (index + i); j++)
					{
						out[j].prev_crossing = 0;
						out[j].next_crossing = crossing.index;
						out[j].estimated_size = size;
					}

					filled_start_frames = true;
				}
				else
				{
					const auto distance = crossing.index - zx.latest;

					for (std::uint32_t j = 0; j <= distance; j++)
					{
						const auto x = float(j) / distance;

						out[j + zx.latest].prev_crossing = zx.latest;
						out[j + zx.latest].next_crossing = crossing.index;
						out[j + zx.latest].estimated_size = blink::math::lerp(prev_size, size, x);
					}
				}

				out[crossing.index].prev_crossing = crossing.index;
				out[crossing.index].estimated_size = size;

				callbacks.report_progress(float((index + i)) / (n - 1));

				prev_size = size;
				zx.latest = crossing.index;
				zx.best_peak = 0.0f;
			}
		}

		index += num_chunk_frames;
		frames_remaining -= num_chunk_frames;
	}

	for (std::uint64_t i = zx.latest; i < n; i++)
	{
		out[i].prev_crossing = zx.latest;
		out[i].next_crossing = n;
		out[i].estimated_size = prev_size;
	}

	return true;
}

}