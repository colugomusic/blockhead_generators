#include <blink.h>
#include <blink/synth.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPFilters.h>
#include <DSP/MLDSPGens.h>
#pragma warning(pop)

class Test;


class Phasor
{
	std::int32_t phase_ = 0;
	bool up_ = false;
	float sync_;

	void update_phase(std::int32_t inc, float sync)
	{
		if (sync > 0.0f)
		{
			phase_ = std::int32_t(sync * inc);
		}
		else
		{
			phase_ += inc;
		}
	}

	void update_sync_value(std::int32_t prev_phase, std::int32_t inc)
	{
		if (prev_phase <= 0 && phase_ >= 0)
		{
			sync_ = float(phase_) / inc;
			return;
		}

		sync_ = -1.0f;
	}

public:

	void reset()
	{
		phase_ = 0;
		up_ = false;
	}

	float sync() const
	{
		return sync_;
	}

	float operator()(float cyclesPerSample, float sync)
	{
		constexpr float steps_per_cycle(static_cast<float>(ml::const_math::pow(2., 32)));
		constexpr float cycles_per_step(1.f / steps_per_cycle);

		// calculate int steps per sample
		const auto steps_per_sample = cyclesPerSample * steps_per_cycle;
		const auto inc = int(steps_per_sample);
		const auto prev_phase = phase_;

		update_phase(inc, sync);
		update_sync_value(prev_phase, inc);

		// convert counter to float output range
		return float(phase_) * cycles_per_step + 0.5f;
	}
};

static float polyBLEP(float phase, float freq)
{
	auto t = phase;
	const auto dt = freq;

	auto c = 0.0f;

	if (t < dt)
	{
		t = t / dt;
		c = t + t - t * t - 1.0f;
	}
	else if (t > 1.0f - dt)
	{
		t = (t - 1.0f) / dt;
		c = t * t + t + t + 1.0f;
	}
	
	return c;
}

inline float phasor_to_sine(float phase)
{
	constexpr auto sqrt2(static_cast<float>(ml::const_math::sqrt(2.0f)));
	constexpr auto range(sqrt2 - sqrt2 * sqrt2 * sqrt2 / 6.0f);
	constexpr auto scale(1.0f / range);
	constexpr auto domain(sqrt2 * 4.0f);
	constexpr auto flip_offset(sqrt2 * 2.0f);
	constexpr auto one(1.0f);
	constexpr auto one_sixth(1.0f / 6.0f);

	// scale and offset input phasor on (0, 1) to sine approx domain (-sqrt(2), 3*sqrt(2))
	const auto omega = phase * (domain)+(-sqrt2);

	// reverse upper half of phasor to get triangle
	const auto triangle = omega > sqrt2 ? flip_offset - omega : omega;

	return scale * triangle * (one - triangle * triangle * one_sixth);
}

inline float phasor_to_triangle(float phase)
{
	const auto omega = 2.0f * phase;
	const auto triangle = phase > 0.5f ? 2.0f - omega : omega;

	return (2.0f * triangle) - 1.0f;
}

// input: phasor on (0, 1), normalized freq, pulse width
// output: antialiased pulse
inline float phasor_to_pulse(float phase, float freq, float width)
{
	float dummy;

	// get pulse selector mask
	const auto maskV = phase > width;

	// select -1 or 1 (could be a multiply instead?)
	auto pulse = phase > width ? -1.0f : 1.0f;

	// add blep for up-going transition
	pulse += polyBLEP(phase, freq);

	// subtract blep for down-going transition
	const auto down = std::modf(phase - width + 1.0f, &dummy);

	pulse -= polyBLEP(down, freq);

	return pulse;
}

inline float phasor_to_saw(float phase, float freq)
{
	// scale phasor to saw range (-1, 1)
	const auto saw = phase * 2.0f - 1.0f;

	// subtract BLEP from saw to smooth down-going transition
	return saw - polyBLEP(phase, freq);
}

class Oscillator
{
	Phasor phasor_;
	float value_ = 0.0f;

protected:

	float calculate(std::function<float(float)> calculator, float freq, float sync = -1.0f)
	{
		return (value_ = calculator(phasor_(freq, sync)));
	}

public:

	void reset() { phasor_.reset(); }
	float value() const { return value_; }
	float sync() const { return phasor_.sync(); }
};

class SineOsc : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [](float phase)
		{
			return phasor_to_sine(phase);
		};

		return calculate(calculator, freq, sync);
	}
};

class TriangleOsc : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [](float phase)
		{
			return phasor_to_triangle(phase);
		};

		return calculate(calculator, freq, sync);
	}
};

class PulseOsc : public Oscillator
{
public:

	float operator()(float freq, float width, float sync = -1.0f)
	{
		const auto calculator = [freq, width](float phase)
		{
			return phasor_to_pulse(phase, freq, width);
		};

		return calculate(calculator, freq, sync);
	}
};

class SawOsc : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [freq](float phase)
		{
			return phasor_to_saw(phase, freq);
		};

		return calculate(calculator, freq, sync);
	}
};

class MultiWaveOsc : public Oscillator
{
public:

	static constexpr auto WAVE_SINE = 0.0f;
	static constexpr auto WAVE_TRIANGLE = 1.0f / 3.0f;
	static constexpr auto WAVE_PULSE = 2.0f / 3.0f;
	static constexpr auto WAVE_SAW = 1.0f;

	float operator()(float freq, float width, float wave, float sync = -1.0f)
	{
		const auto calculator = [freq, width, wave](float phase)
		{
			constexpr auto one_third = 1.0f / 3.0f;
			constexpr auto two_thirds = 2.0f / 3.0f;

			if (wave < one_third)
			{
				const auto triangle = phasor_to_triangle(phase);
				const auto sine = phasor_to_sine(phase);

				const auto xfade = blink::math::inverse_lerp(0.0f, one_third, wave);

				return blink::math::lerp(sine, triangle, xfade);
			}

			if (wave < two_thirds)
			{
				const auto triangle = phasor_to_triangle(phase);
				const auto pulse = phasor_to_pulse(phase, freq, width);

				const auto xfade = blink::math::inverse_lerp(one_third, two_thirds, wave);

				return blink::math::lerp(triangle, pulse, xfade);
			}

			const auto pulse = phasor_to_pulse(phase, freq, width);
			const auto saw = phasor_to_saw(phase, freq);

			const auto xfade = blink::math::inverse_lerp(two_thirds, 1.0f, wave);

			return blink::math::lerp(pulse, saw, xfade);
		};

		return calculate(calculator, freq, sync);
	}
};

class Audio : public blink::Synth
{
public:

	Audio(const Test* plugin);

	blink_Error process(const blink_SynthBuffer* buffer, float* out) override;
	blink_Error reset() override;

private:
	
	const Test* plugin_;
	float prev_pos_;
	std::array<MultiWaveOsc, 2> oscs_;
};