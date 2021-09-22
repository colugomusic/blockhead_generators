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

	void write(const FrameWriteParams& params, float filtered_value, float value);
	float read(const FrameReadParams& params, float in);

	void prepare_xfade(const FrameReadParams& params);
	void start_fade_in(const FrameReadParams& params);
	void start_xfade(const FrameReadParams& params);
	float do_xfade(const FrameReadParams& params);
	float do_wet(const FrameReadParams& params);

	const Plugin* plugin_;

	struct Span
	{
		float* buffer { nullptr };
		size_t size { 0 };

		float read(float pos) const;
	};

	std::array<std::vector<float>, 4> buffers_;

	struct
	{
		Span span;
		bool up { false };
		int counter { 0 };
		ml::Lopass filter;
	} write_;
	
	struct
	{
		Span span;
	} stage_;
	
	struct
	{
		Span span;
		float frame { 0.0f };
	} source_;
	
	struct
	{
		Span span;
		float frame { 0.0f };
	} target_;

	struct
	{
		float source_speed_0 { 1.0f };
		float source_speed_1 { 1.0f };
		float target_speed_0 { 1.0f };
		float target_speed_1 { 1.0f };
		bool active { false };
		size_t length { 64 };
		size_t index { 0 };
	} xfade_;

	struct
	{
		bool active { false };
		size_t length { 64 };
		size_t index { 0 };
	} fade_in_;

	int init_ { 0 };
};

} // wavebender