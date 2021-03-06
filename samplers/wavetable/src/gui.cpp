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

	const blink_WarpPoints* warp_points;
};

static Data get_data(const blink_SamplerBuffer* buffer)
{
	Data out;

	out.sliders.amp = Plugin::get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Amp));
	out.sliders.speed = Plugin::get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Speed));
	out.sliders.sample_offset = Plugin::get_int_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_SampleOffset));
	out.toggles.loop = Plugin::get_toggle_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Tog_Loop));
	out.toggles.reverse = Plugin::get_toggle_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Tog_Reverse));
	out.envelopes.amp = Plugin::get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Amp));
	out.envelopes.speed = Plugin::get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Speed));
	out.warp_points = buffer->warp_points;

	return out;
}

static void calculate_positions(
	const Data& data,
	const blink_SampleInfo& sample_info,
	const blink_SR song_rate,
	const blink::Traverser& block_traverser,
	blink::std_traversers::Fudge* fudge_traverser,
	int index,
	int count,
	blink_SamplerDrawInfo* out)
{
	ml::DSPVector sculpted_block_positions;
	ml::DSPVector warped_block_positions;
	ml::DSPVector derivatives;

	fudge_traverser->get_positions(
		data.sliders.speed ? data.sliders.speed->value : 1,
		data.envelopes.speed,
		data.warp_points,
		block_traverser,
		data.sliders.sample_offset ? data.sliders.sample_offset->value : 0,
		count,
		&sculpted_block_positions,
		&warped_block_positions,
		&derivatives);

	auto sculpted_sample_positions = sculpted_block_positions / (float(song_rate) / sample_info.SR);
	auto warped_sample_positions = warped_block_positions / (float(song_rate) / sample_info.SR);
	auto final_sample_positions = warped_sample_positions;

	if (data.toggles.loop && data.toggles.loop->value)
	{
		for (int i = 0; i < count; i++)
		{
			if (final_sample_positions[i] > sample_info.num_frames - 1)
			{
				final_sample_positions[i] = float(std::fmod(final_sample_positions[i], sample_info.num_frames));
			}
		}
	}

	if (data.toggles.reverse && data.toggles.reverse->value)
	{
		final_sample_positions = float(sample_info.num_frames - 1) - final_sample_positions;
	}

	if (out->sculpted_block_positions)
	{
		std::copy(sculpted_block_positions.getConstBuffer(), sculpted_block_positions.getConstBuffer() + count, out->sculpted_block_positions + index);
	}

	if (out->sculpted_sample_positions)
	{
		std::copy(sculpted_sample_positions.getConstBuffer(), sculpted_sample_positions.getConstBuffer() + count, out->sculpted_sample_positions + index);
	}

	if (out->warped_block_positions)
	{
		std::copy(warped_block_positions.getConstBuffer(), warped_block_positions.getConstBuffer() + count, out->warped_block_positions + index);
	}

	if (out->warped_sample_positions)
	{
		std::copy(warped_sample_positions.getConstBuffer(), warped_sample_positions.getConstBuffer() + count, out->warped_sample_positions + index);
	}

	if (out->final_sample_positions)
	{
		std::copy(final_sample_positions.getConstBuffer(), final_sample_positions.getConstBuffer() + count, out->final_sample_positions + index);
	}

	if (out->waveform_derivatives)
	{
		std::copy(derivatives.getConstBuffer(), derivatives.getConstBuffer() + count, out->waveform_derivatives + index);
	}
}

static void calculate_amp(const Wavetable* plugin, const Data& data, const blink::BlockPositions& block_positions, float* out)
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

blink_Error GUI::draw(const Wavetable* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
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

		traverser_resetter_.check(data.envelopes.speed, &block_traverser_);

		calculate_positions(
			data,
			*buffer->sample_info,
			buffer->song_rate,
			block_traverser_,
			&position_traverser_,
			index,
			count,
			out);

		calculate_amp(plugin, data, block_positions, out->amp + index);

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}