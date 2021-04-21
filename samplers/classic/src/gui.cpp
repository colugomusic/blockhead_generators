#include "gui.h"
#include "plugin.h"

using namespace blink;

struct Data
{
	struct Sliders
	{
		const blink_SliderData* amp;
		const blink_SliderData* pitch;
		const blink_IntSliderData* sample_offset;
	} sliders;

	struct Toggles
	{
		const blink_ToggleData* loop;
		const blink_ToggleData* reverse;
	} toggles;

	struct Envelopes
	{
		const blink_EnvelopeData* amp;
		const blink_EnvelopeData* pitch;
	} envelopes;
};

static Data get_data(const blink_SamplerBuffer* buffer)
{
	Data out;

	out.sliders.amp = Plugin::get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Amp));
	out.sliders.pitch = Plugin::get_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_Pitch));
	out.sliders.sample_offset = Plugin::get_int_slider_data(buffer->parameter_data, int(Classic::ParameterIndex::Sld_SampleOffset));
	out.toggles.loop = Plugin::get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Loop));
	out.toggles.reverse = Plugin::get_toggle_data(buffer->parameter_data, int(Classic::ParameterIndex::Tog_Reverse));
	out.envelopes.amp = Plugin::get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Amp));
	out.envelopes.pitch = Plugin::get_envelope_data(buffer->parameter_data, int(Classic::ParameterIndex::Env_Pitch));

	return out;
}

static void calculate_positions(
	const Data& data,
	const blink_SampleInfo& sample_info,
	const blink_SR song_rate,
	const blink::Traverser& block_traverser,
	blink::std_traversers::Classic* classic_traverser,
	int count,
	float* out_positions,
	float* out_derivatives)
{
	ml::DSPVector derivatives_vec;

	auto positions =
		classic_traverser->get_positions(
			data.sliders.pitch ? data.sliders.pitch->value : 0,
			data.envelopes.pitch,
			block_traverser,
			data.sliders.sample_offset ? data.sliders.sample_offset->value : 0,
			count,
			out_derivatives ? derivatives_vec.getBuffer() : nullptr);

	positions /= (float(song_rate) / sample_info.SR);

	if (data.toggles.loop && data.toggles.loop->value)
	{
		for (int i = 0; i < count; i++)
		{
			if (positions[i] > sample_info.num_frames - 1)
			{
				positions[i] = float(std::fmod(positions[i], sample_info.num_frames));
			}
		}
	}

	if (data.toggles.reverse && data.toggles.reverse->value)
	{
		positions = float(sample_info.num_frames - 1) - positions;
	}

	std::copy(positions.getConstBuffer(), positions.getConstBuffer() + count, out_positions);

	if (out_derivatives) std::copy(derivatives_vec.getConstBuffer(), derivatives_vec.getConstBuffer() + count, out_derivatives);
}

static void calculate_amp(const Classic* plugin, const Data& data, const blink::BlockPositions& block_positions, float* out)
{
	ml::DSPVector amp(1.0f);

	if (data.envelopes.amp)
	{
		plugin->env_amp().search_vec(data.envelopes.amp, block_positions, amp.getBuffer());
	}

	if (data.sliders.amp)
	{
		amp *= data.sliders.amp->value;
	}

	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + block_positions.count, out);
}

blink_Error GUI::draw(const Classic* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	block_traverser_.set_reset(0);

	const auto data = get_data(buffer);

	auto frames_remaining = n;
	int index = 0;

	BlockPositions block_positions;

	while (frames_remaining > 0 && frames_remaining <= buffer->sample_info->num_frames)
	{
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		block_positions(buffer->positions + index, buffer->data_offset, count);

		block_traverser_.generate(block_positions, count);

		traverser_resetter_.check(data.envelopes.pitch, &block_traverser_);

		calculate_positions(
			data,
			*buffer->sample_info,
			buffer->song_rate,
			block_traverser_,
			&position_traverser_,
			count,
			out->waveform_positions + index,
			out->waveform_derivatives ? out->waveform_derivatives + index : nullptr);

		calculate_amp(plugin, data, block_positions, out->amp + index);

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}