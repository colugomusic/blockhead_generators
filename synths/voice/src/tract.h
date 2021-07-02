#pragma once

#include <random>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

class Tract
{
public:

	struct Input
	{
		ml::DSPVector glottal_output;
		ml::DSPVector fricative_noise;
		ml::DSPVector fricative_intensity;
		ml::DSPVector index;
		ml::DSPVector diameter;

		struct Tongue
		{
			ml::DSPVector index;
			ml::DSPVector diameter;
		} tongue;
	};

	ml::DSPVector operator()(int SR, const Input& input);
	void reset();

private:

	static constexpr int N = 44;
	static constexpr int NOSE_LENGTH = 28;
	static constexpr int NOSE_START = N - NOSE_LENGTH + 1;
	static constexpr int BLADE_START = 10;
	static constexpr int TIP_START = 32;
	static constexpr int LIP_START = 39;
	static constexpr int MOVEMENT_SPEED = 15; // 15
	static constexpr double NOSE_OFFSET = 0.8f;

	static constexpr int TONGUE_LOWER_INDEX_BOUND = BLADE_START + 2;
	static constexpr int TONGUE_UPPER_INDEX_BOUND = TIP_START - 3;
	static constexpr double TONGUE_INDEX_CENTER = 0.5f * (TONGUE_LOWER_INDEX_BOUND + TONGUE_UPPER_INDEX_BOUND);

	class Step
	{
	public:

		struct Input
		{
			double glottal_output;
			double fricative_noise;
			double fricative_intensity;
			double index;
			double diameter;

			struct Tongue
			{
				double index;
				double diameter;
			} tongue;
		};

		Step();

		double operator()(int SR, double lambda, const Input& input);
		void finish_block(int SR);
		void reset();

	private:

		void configure(const Input& input);
		void set_rest_diameter(const Input& input);
		void add_transient(int position);
		void process_transients(int SR);
		void add_turbulence_noise(const Input& input);
		void add_turbulence_noise_at_index(double noise, double index, double diameter);
		void calculate_reflections();
		void calculate_nose_reflections();
		void reshape_tract(double delta_time);

		struct Transients
		{
			struct Transient
			{
				int position;
				double time_alive;
				double life_time;
				double strength;
				double exponent;
			};

			std::array<Transient, 32> list;
			int count = 0;
		} transients_;

		std::array<double, N> a_;
		std::array<double, N> diameter_;
		std::array<double, N> rest_diameter_;
		std::array<double, N> target_diameter_;
		std::array<double, N> new_diameter_;
		std::array<double, N> L_;
		std::array<double, N> R_;
		std::array<double, N + 1> junction_output_L_;
		std::array<double, N + 1> junction_output_R_;
		std::array<double, N + 1> reflection_;
		std::array<double, N + 1> new_reflection_;
		std::array<double, NOSE_LENGTH> nose_a_;
		std::array<double, NOSE_LENGTH> nose_L_;
		std::array<double, NOSE_LENGTH> nose_R_;
		std::array<double, NOSE_LENGTH> nose_diameter_;
		std::array<double, NOSE_LENGTH + 1> nose_junction_output_L_;
		std::array<double, NOSE_LENGTH + 1> nose_junction_output_R_;
		std::array<double, NOSE_LENGTH + 1> nose_reflection_;

		int last_obstruction_ = -1;
		//double tongue_index_ = 12.9;
		//double tongue_diameter_ = 2.43;
		double grid_offset_ = 1.7;
		double fade_ = 1.0;
		double reflection_L_ = 0.0;
		double reflection_R_ = 0.0;
		double reflection_nose_ = 0.0;
		double new_reflection_L_ = 0.0;
		double new_reflection_R_ = 0.0;
		double new_reflection_nose_ = 0.0;
		double glottal_reflection_ = 0.75;
		double lip_reflection_ = -0.85;
		double velum_target_ = 0.01;
	};

	Step step_;
};