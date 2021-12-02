#include <blink.h>
#include <blink/effect_unit.hpp>
#include <snd/audio/env_follower.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

namespace splatter {

class Plugin;
class Instance;

template <size_t ROWS>
class ZeroX
{
public:

	ZeroX()
	{
		up_.fill(false);
	}

	ml::DSPVectorArrayInt<ROWS> operator()(const ml::DSPVectorArray<ROWS>& in)
	{
		ml::DSPVectorArrayInt<ROWS> out;

		for (int row = 0; row < ROWS; row++)
		{
			const auto& in_row = in.constRow(row);
			auto& out_row = out.row(row);

			for (int i = 0; i < kFloatsPerDSPVector; i++)
			{
				if (up_[row])
				{
					if (in_row[i] < 0.0f)
					{
						out_row[i] = -1;
						up_[row] = false;
					}
				}
				else
				{
					if (in_row[i] > 0.0f)
					{
						out_row[i] = 1;
						up_[row] = true;
					}
				}
			}
		}

		if (!init_)
		{
			for (int row = 0; row < ROWS; row++)
			{
				out.row(row)[0] = in.constRow(row)[0] > 0.0f ? 1 : -1;
			}

			init_ = true;
		}

		return out;
	}

private:

	bool init_ { false };
	std::array<bool, ROWS> up_;
};

template <size_t ROWS>
class Smoother
{
public:

	Smoother()
	{
		a_.fill(0.0f);
	}

	ml::DSPVectorArray<ROWS> operator()(const ml::DSPVectorArray<ROWS>& in, const ml::DSPVector& samples)
	{
		ml::DSPVectorArray<ROWS> out;
		
		const auto speed = 1.0f / samples;

		for (int row = 0; row < ROWS; row++)
		{
			const auto& in_row = in.constRow(row);
			auto& out_row = out.row(row);

			for (int i = 0; i < kFloatsPerDSPVector; i++)
			{
				a_[row] = (std::min(1.0f, speed[i]) * (in_row[i] - a_[row])) + a_[row];

				out_row[i] = a_[row];
			}
		}

		return out;
	}

private:

	std::array<float, ROWS> a_;
};

template <size_t ROWS>
class FreqDetector
{
public:

	ml::DSPVectorArray<ROWS> operator()(const ml::DSPVector& SR, const ml::DSPVectorArrayInt<ROWS>& zero_crossings)
	{
		ml::DSPVectorArray<ROWS> out;

		for (int row = 0; row < ROWS; row++)
		{
			const auto& zero_crossings_row = zero_crossings.constRow(row);
			auto& out_row = out.row(row);

			ml::DSPVectorInt sample_count;

			for (int i = 0; i < kFloatsPerDSPVector; i++)
			{
				if (zero_crossings_row[i] > 0)
				{
					sample_count_[row] = counter_[row];
					counter_[row] = 0;
				}

				sample_count[i] = sample_count_[row];
				counter_[row]++;
			}

			out_row = SR / ml::intToFloat(sample_count);
		}

		return out;
	}

	void reset()
	{
		counter_.fill(0);
		sample_count_.fill(0);
	}

private:

	std::array<int, ROWS> counter_;
	std::array<int, ROWS> sample_count_;
};

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:
	
	const Plugin* plugin_;
	snd::audio::EnvFollowerAR<2> follower_;
	std::array<ml::Lopass, 2> lp_;
	ZeroX<2> zero_x_;
	FreqDetector<2> freq_detector_;
	Smoother<2> freq_smoother_;
	std::array<ml::SineGen, 2> sine_;
};

} // splatter
