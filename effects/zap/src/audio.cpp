#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"

using namespace blink;

namespace zap {

static constexpr auto FREQ_GLIDE_TIME_MS = 25.0f;
static constexpr auto MIN_FREQ           = 130.0f;
static constexpr auto MAX_FREQ           = 4000.0f;

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

auto Audio::stream_init() -> void {
	const auto glide_samples = static_cast<float>(SR()) * (FREQ_GLIDE_TIME_MS / 1000);
	for (auto& glide : frequency_glide_) {
		glide.setGlideTimeInSamples(glide_samples);
	}
}

blink_Error Audio::process(const blink_EffectBuffer& buffer, const blink_EffectUnitState& unit_state, const float* in, float* out) {
	AudioData data(*plugin_, unit_state.parameter_data);

	auto base_freq = data.envelopes.frequency.search(block_positions());
	auto spread    = data.envelopes.spread.search(block_positions());
	const auto res = data.envelopes.resonance.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	const auto transform_spread = [](float x) { return x * x * std::signbit(x); };

	base_freq = math::convert::linear_to_filter_hz(base_freq);
	spread    = transform_spread(spread);

	const auto offset = spread * 1000.0f;

	std::array<float, 2> freq;
	freq[0] = std::clamp(base_freq - offset, MIN_FREQ, MAX_FREQ);
	freq[1] = std::clamp(base_freq + offset, MIN_FREQ, MAX_FREQ);

	ml::DSPVectorArray<2> freq_vec;

	for (int i = 0; i < 2; i++) {
		freq_vec.row(i) = frequency_glide_[i](freq[i]);
	}

	ml::DSPVectorArray<2> in_vec(in);

	auto out_vec = filter_(in_vec, SR(), freq_vec, ml::repeatRows<2>(res));

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset() {
	filter_.clear();
}

} // zap
