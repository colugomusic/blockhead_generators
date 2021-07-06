#pragma once

#include <array>
#include <cassert>
#include <functional>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

template <size_t ROWS>
class Resampler
{
public:

	using Source = std::function<ml::DSPVectorArray<ROWS>()>;

	ml::DSPVectorArray<ROWS> operator()(Source source, float factor)
	{
		ml::DSPVectorArray<ROWS> out;

		for (int i = 0; i < kFloatsPerDSPVector; i++)
		{
			const auto frame = read_source_block_frame(source, frame_pos_);

			frame_pos_ += factor;

			for (int r = 0; r < ROWS; r++)
			{
				out.row(r)[i] = frame[r];
			}
		}

		return out;
	}

private:

	std::array<float, ROWS> read_source_block_frame(Source source, double pos)
	{
		const auto idx_prev = std::uint32_t(std::floor(pos));
		const auto idx_next = std::uint32_t(std::ceil(pos));
		const auto x = pos - idx_prev;

		const auto prev = read_source_block_frame(source, idx_prev);
		const auto next = read_source_block_frame(source, idx_next);

		std::array<float, ROWS> out;

		for (int r = 0; r < ROWS; r++)
		{
			out[r] = ml::lerp(prev[r], next[r], float(x));
		}

		return out;
	}
	
	std::array<float, ROWS> read_source_block_frame(Source source, std::uint32_t index)
	{
		const auto block_index = index / kFloatsPerDSPVector;
		const auto local_index = index % kFloatsPerDSPVector;

		assert(block_index >= curr_block_index_ - 1);
		assert(block_index <= curr_block_index_ + 1);

		if (block_index < curr_block_index_)
		{
			return read_source_block_frame(prev_source_block_, local_index);
		}

		if (block_index > curr_block_index_)
		{
			read_next_source_block(source);
		}
		
		return read_source_block_frame(curr_source_block_, local_index);
	}

	std::array<float, ROWS> read_source_block_frame(const ml::DSPVectorArray<ROWS>& block, int index)
	{
		std::array<float, ROWS> out;

		for (int r = 0; r < ROWS; r++)
		{
			out[r] = block.constRow(r)[index];
		}

		return out;
	}

	void read_next_source_block(Source source)
	{
		prev_source_block_ = curr_source_block_;
		curr_source_block_ = source();
		curr_block_index_++;
	}

	double frame_pos_ = 0.0;
	ml::DSPVectorArray<ROWS> prev_source_block_;
	ml::DSPVectorArray<ROWS> curr_source_block_;
	std::int64_t curr_block_index_ = -1;
};