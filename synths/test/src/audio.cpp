#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"

using namespace blink;

namespace test {

Audio::Audio(Instance* instance)
	: SynthUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_SynthBuffer& buffer, const blink_SynthUnitState& unit_state, float* out)
{
	AudioData data(*plugin_, unit_state.parameter_data);

	const auto amp = data.envelopes.amp.search_vec(block_positions());
 	const auto wave = data.envelopes.wave.search_vec(block_positions());
	const auto p0 = data.envelopes.p0.search_vec(block_positions()) + 60.0f;
	const auto p1 = data.envelopes.p1.search_vec(block_positions()) + 60.0f;
	const auto fm0 = data.envelopes.fm0.search_vec(block_positions());
	const auto fm1 = data.envelopes.fm1.search_vec(block_positions());
	
	const auto freq0 = blink::math::convert::pitch_to_frequency(p0);
	const auto freq1 = blink::math::convert::pitch_to_frequency(p1);

	ml::DSPVector osc_out;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		const auto fm0_freq = std::max(((fm0[i] * fm0[i] * 20000.0f) * oscs_[1].value()) + freq0[i], 0.0f);
		const auto fm1_freq = std::max(((fm1[i] * fm1[i] * 20000.0f) * oscs_[0].value()) + freq1[i], 0.0f);

		osc_out[i] = oscs_[0](fm0_freq / SR(), 0.5f, wave[i]);

		oscs_[1](fm1_freq / SR(), 0.5f, bleat::oscillators::scalar::MultiWaveOsc::WAVE_SINE);
	}

	auto out_vec = ml::repeatRows<2>(osc_out);

	out_vec =
		noise_gen_(
			out_vec,
			data.options.noise_mode,
			data.envelopes.noise_amount,
			data.envelopes.noise_color,
			data.sliders.noise_width.value,
			block_positions());

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	oscs_[0].reset();
	oscs_[1].reset();
}

} // test