#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"
#include <blink/dsp.hpp>

using namespace blink;

namespace fudge {

Audio::Audio(Instance* instance)
	: SamplerUnit(instance)
	, plugin_(instance->get_plugin())
	, controller_(plugin_)
	, particles_{ controller_, { controller_, 1 }, { controller_, 2 }, { controller_, 3 } }
{
}

blink_Error Audio::process(const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	AudioData data(*plugin_, unit_state);

	block_traverser_.generate(unit_state.id, block_positions(), kFloatsPerDSPVector);

	const auto analysis_data { buffer.analysis_ready ? plugin_->get_analysis_data(buffer.sample_info->id) : nullptr };

	controller_.process(data, buffer, unit_state, analysis_data, block_traverser_, block_positions(), unit_state.data_offset, SR());

	const auto harmonic_amount { data.envelopes.harmonics_amount.search_vec(block_positions()) };

	ml::DSPVector total_amp;

	for (int i = 0; i < 4; i++)
	{
		auto& particle = particles_[i];

		ml::DSPVector amp;

		if (i == 0)
		{
			amp = 1.0f;
		}
		else
		{
			amp = ml::clamp(harmonic_amount - float(i - 1), ml::DSPVector(0.0f), ml::DSPVector(1.0f));
		}

		total_amp += amp;
		out_vec += particles_[i].process(amp);
	}

	out_vec /= ml::repeatRows<2>(total_amp);

	out_vec =
		noise_gen_(
			out_vec,
			float(data.options.noise_mode.value),
			data.envelopes.noise_amount,
			data.envelopes.noise_color,
			data.sliders.noise_width.value,
			block_positions());

	out_vec = stereo_pan(out_vec, data.sliders.pan.value, data.envelopes.pan, block_positions());

	const auto amp = data.envelopes.amp.search_vec(block_positions()) * data.sliders.amp.value;

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

}
