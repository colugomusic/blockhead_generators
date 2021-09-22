#include <array>
#include <vector>
#include <blink.h>
#include <blink/effect_unit.hpp>
#include <blink/math.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPBuffer.h>
#include <DSP/MLDSPFilters.h>
#pragma warning(pop)

namespace wavebender {

class Plugin;
class Instance;

class Audio : public blink::EffectUnit
{
public:

	Audio(Instance* instance);

	blink_Error process(const blink_EffectBuffer* buffer, const float* in, float* out) override;
	void reset() override;

private:

	enum class CrossfadeMode
	{
		Static,
		Dynamic,
	};

	struct FrameWriteParams
	{
		int bubble { 0 };
	};

	struct FrameReadParams
	{
		float tilt { 0.0f };
		float ff { 1.0f };
		float crossfade_size { 1.0f };
		CrossfadeMode crossfade_mode { CrossfadeMode::Dynamic };
	};

	void stream_init() override;

	struct Channel
	{
		float operator()(const FrameWriteParams& write_params, const FrameReadParams& read_params, float in, float filtered_in);
		void do_write(const FrameWriteParams& params, float filtered_value, float value);
		float do_read(const FrameReadParams& params, float in);

		void prepare_xfade(const FrameReadParams& params);
		void start_fade_in(const FrameReadParams& params);
		void start_xfade(const FrameReadParams& params);
		float do_xfade(const FrameReadParams& params);
		float do_wet(const FrameReadParams& params);

		std::array<std::vector<float>, 4> buffers;

		struct Span
		{
			float* buffer { nullptr };
			size_t size { 0 };

			float read(float pos) const;
		};

		struct
		{
			Span span;
			bool up { false };
			int counter { 0 };
			ml::Lopass filter;
		} write;
		
		struct
		{
			Span span;
		} stage;
		
		struct
		{
			Span span;
			float frame { 0.0f };
		} source;
		
		struct
		{
			Span span;
			float frame { 0.0f };
		} target;

		struct
		{
			float source_speed_0 { 1.0f };
			float source_speed_1 { 1.0f };
			float target_speed_0 { 1.0f };
			float target_speed_1 { 1.0f };
			bool active { false };
			size_t length { 64 };
			size_t index { 0 };
		} xfade;

		struct
		{
			bool active { false };
			size_t length { 64 };
			size_t index { 0 };
		} fade_in;

		int init { 0 };
	};

	const Plugin* plugin_;

	std::array<Channel, 2> channels_;
};

} // wavebender