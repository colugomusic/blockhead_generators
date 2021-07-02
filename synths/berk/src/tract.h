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
	static constexpr float NOSE_OFFSET = 0.8f;

	static constexpr int TONGUE_LOWER_INDEX_BOUND = BLADE_START + 2;
	static constexpr int TONGUE_UPPER_INDEX_BOUND = TIP_START - 3;
	static constexpr float TONGUE_INDEX_CENTER = 0.5f * (TONGUE_LOWER_INDEX_BOUND + TONGUE_UPPER_INDEX_BOUND);

	class Step
	{
	public:

		struct Input
		{
			float glottal_output;
			float fricative_noise;
			float fricative_intensity;
			float index;
			float diameter;

			struct Tongue
			{
				float index;
				float diameter;
			} tongue;
		};

		Step();

		float operator()(int SR, float lambda, const Input& input);
		void finish_block(int SR);
		void reset();

	private:

		void configure(const Input& input);
		void set_rest_diameter(const Input& input);
		void add_transient(int position);
		void process_transients(int SR);
		void add_turbulence_noise(const Input& input);
		void add_turbulence_noise_at_index(float noise, float index, float diameter);
		void calculate_reflections();
		void calculate_nose_reflections();
		void reshape_tract(float delta_time);

		struct Transients
		{
			struct Transient
			{
				int position;
				float time_alive;
				float life_time;
				float strength;
				float exponent;
			};

			std::array<Transient, 32> list;
			int count = 0;
		} transients_;

		std::array<float, N> a_;
		std::array<float, N> diameter_;
		std::array<float, N> rest_diameter_;
		std::array<float, N> target_diameter_;
		std::array<float, N> new_diameter_;
		std::array<float, N> L_;
		std::array<float, N> R_;
		std::array<float, N + 1> junction_output_L_;
		std::array<float, N + 1> junction_output_R_;
		std::array<float, N + 1> reflection_;
		std::array<float, N + 1> new_reflection_;
		std::array<float, NOSE_LENGTH> nose_a_;
		std::array<float, NOSE_LENGTH> nose_L_;
		std::array<float, NOSE_LENGTH> nose_R_;
		std::array<float, NOSE_LENGTH> nose_diameter_;
		std::array<float, NOSE_LENGTH + 1> nose_junction_output_L_;
		std::array<float, NOSE_LENGTH + 1> nose_junction_output_R_;
		std::array<float, NOSE_LENGTH + 1> nose_reflection_;

		int last_obstruction_ = -1;
		float grid_offset_ = 1.7f;
		float fade_ = 1.0f;
		float reflection_L_ = 0.0f;
		float reflection_R_ = 0.0f;
		float reflection_nose_ = 0.0f;
		float new_reflection_L_ = 0.0f;
		float new_reflection_R_ = 0.0f;
		float new_reflection_nose_ = 0.0f;
		float glottal_reflection_ = 0.75f;
		float lip_reflection_ = -0.85f;
		float velum_target_ = 0.01f;
	};

	Step step_;
};