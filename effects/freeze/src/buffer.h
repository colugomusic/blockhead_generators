#pragma once

#include <array>
#include <cstdint>

#include <blink/math.hpp>

namespace freeze {

template <std::size_t SIZE>
class Buffer
{
public:

	Buffer()
	{
		for (int row{}; row < 2; row++)
		{
			data_[row].fill(0.0f);
		}
	}

	static bool not_zero(float L, float R, float threshold = 0.0001f)
	{
		return L > threshold || L < -threshold || R > threshold || R < -threshold;
	}

	void process(float L, float R)
	{
		if (!init_ && not_zero(L, R))
		{
			init_ = true;
		}

		if (init_)
		{
			write(L, R);
		}
	}

	float at(std::size_t row, int index) const
	{
		if (index > size_ - 1)
		{
 			return 0.0f;
		}

		return data_[row][index];
	}

	void clear()
	{
		init_ = false;
		size_ = 0;
	}

	int frames_available() const { return size_; }

private:

	bool write(float L, float R)
	{
		if (size_ >= SIZE) return false;

		data_[0][size_] = L;
		data_[1][size_] = R;

		size_++;

		return true;
	}

	std::array<std::array<float, SIZE>, 2> data_;

	bool init_ = false;
	int size_ = 0;
};

using FreezeBuffer = Buffer<8192*2>;

} // freeze