#include "gui.h"
#include "plugin.h"

using namespace blink;

struct Data
{
	struct Sliders
	{
		const blink_SliderData* amp;
		const blink_SliderData* speed;
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
		const blink_EnvelopeData* speed;
	} envelopes;
};

static Data get_data(const blink_SamplerBuffer* buffer)
{
	Data out;

	out.sliders.amp = Plugin::get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Amp));
	out.sliders.speed = Plugin::get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Speed));
	out.sliders.sample_offset = Plugin::get_int_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_SampleOffset));
	out.toggles.loop = Plugin::get_toggle_data(buffer->parameter_data, int(Fudge::ParameterIndex::Tog_Loop));
	out.toggles.reverse = Plugin::get_toggle_data(buffer->parameter_data, int(Fudge::ParameterIndex::Tog_Reverse));
	out.envelopes.amp = Plugin::get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Amp));
	out.envelopes.speed = Plugin::get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Speed));

	return out;
}

static void calculate_positions(
	const Data& data,
	const blink_SampleInfo& sample_info,
	const blink_SR song_rate,
	const blink::Traverser& block_traverser,
	blink::std_traversers::Fudge* fudge_traverser,
	int count,
	float* out_positions,
	float* out_derivatives)
{
	ml::DSPVector derivatives_vec;

	auto positions =
		fudge_traverser->get_positions(
			data.sliders.speed ? data.sliders.speed->value : 1,
			data.envelopes.speed,
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

static void calculate_amp(const Fudge* plugin, const Data& data, const float* positions, int count, float prev_pos, float* out)
{
	ml::DSPVector amp(1.0f);

	if (data.envelopes.amp)
	{
		plugin->env_amp().search_vec(data.envelopes.amp, positions, count, prev_pos, amp.getBuffer());
	}

	if (data.sliders.amp)
	{
		amp *= data.sliders.amp->value;
	}

	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + count, out);
}

blink_Error GUI::get_waveform_positions(const Fudge* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp)
{
	block_traverser_.set_reset(0);

	const auto data = get_data(buffer);

	auto frames_remaining = n;
	int index = 0;
	auto prev_pos = std::numeric_limits<float>::max();

	while (frames_remaining > 0 && frames_remaining <= buffer->sample_info->num_frames)
	{
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		block_traverser_.generate(buffer->positions + index, count, buffer->data_offset);

		traverser_resetter_.check(data.envelopes.speed, &block_traverser_);

		calculate_positions(
			data,
			*buffer->sample_info,
			buffer->song_rate,
			block_traverser_,
			&position_traverser_,
			count,
			out + index,
			derivatives ? derivatives + index : nullptr);

		const auto positions = block_traverser_.get_read_position().getConstBuffer();

		calculate_amp(plugin, data, positions, count, prev_pos, amp + index);

		prev_pos = positions[count - 1];
		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}