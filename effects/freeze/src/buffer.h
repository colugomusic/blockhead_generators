#pragma once

#include <array>
#include <cstdint>

#include <blink/math.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

template <std::int32_t COLUMNS>
class Buffer
{
public:

	void process(int vector_index, bool reset, float L, float R)
	{
		if (reset)
		{
			init_ = false;
			size_ = 0;
			zero_index_ = 0;
			frames_available_ = 0;
		}

		if (!init_)
		{
			if (L > 0.0001f || L < -0.0001f || R > 0.0001f || R < -0.0001f)
			{
				init_ = true;
				zero_index_ = vector_index + size_;
			}
		}

		if (init_ && write(L, R))
		{
			frames_available_ = size_ - zero_index_;
		}
	}

	float at(std::size_t row, int index) const
	{
		if (index > frames_available_ - 1)
		{
 			return 0.0f;
		}

		index += zero_index_;

		const auto column_index = int(index / kFloatsPerDSPVector);

		if (column_index > size_ - 1)
		{
			return 0.0f;
		}

		const auto sub_index = int(index % kFloatsPerDSPVector);

		return data_.constRow(int(row * COLUMNS) + column_index)[sub_index];
	}

	void clear()
	{
		init_ = false;
		size_ = 0;
		zero_index_ = 0;
		frames_available_ = 0;
	}

	int frames_available() const { return frames_available_; }

private:

	bool write(float L, float R)
	{
		if (size_ >= COLUMNS * kFloatsPerDSPVector) return false;

		const auto index = size_;
		const auto column_index = int(index / kFloatsPerDSPVector);
		const auto sub_index = int(index % kFloatsPerDSPVector);

		data_.row(int(0 * COLUMNS) + column_index)[sub_index] = L;
		data_.row(int(1 * COLUMNS) + column_index)[sub_index] = R;

		size_++;

		return true;
	}

	ml::DSPVectorArray<2 * COLUMNS> data_;

	bool init_ = false;
	int size_ = 0;
	int zero_index_ = 0;
	int frames_available_ = 0;
};

using FreezeBuffer = Buffer<128>;