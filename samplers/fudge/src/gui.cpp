#include "gui.h"
#include "plugin.h"
#include <blink/envelope_data.hpp>
#include <blink/sample_data.hpp>
#include <blink/slider_data.hpp>

using namespace blink;

namespace fudge {

struct Data
{
	struct Sliders
	{
		Sliders(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: amp(plugin, plugin->params().sliders.amp->slider(), buffer->parameter_data)
			, speed(plugin, plugin->params().sliders.speed->slider(), buffer->parameter_data)
			, sample_offset(plugin->get_int_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_SampleOffset))->value)
		{
		}

		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Speed)> speed;

		int sample_offset;
	} sliders;

	struct Toggles
	{
		Toggles(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: loop(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Loop))->data.points[0].value == BLINK_TRUE)
			, reverse(plugin->get_toggle_data(buffer->parameter_data, int(Parameters::Index::Tog_Reverse))->data.points[0].value == BLINK_TRUE)
		{
		}

		bool loop;
		bool reverse;
	} toggles;

	struct Envelopes
	{
		Envelopes(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
			, speed(plugin, plugin->params().env.speed->envelope(), buffer->parameter_data)
		{
		}

		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Speed)> speed;
	} envelopes;

	const blink_WarpPoints* warp_points;

	Data(const Plugin* plugin, const blink_SamplerBuffer* buffer)
		: sliders(plugin, buffer)
		, toggles(plugin, buffer)
		, envelopes(plugin, buffer)
		, warp_points(buffer->warp_points)
	{
	}
};

static void calculate_positions(
	const Data& data,
	const SampleData& sample_data,
	const blink_SR song_rate,
	const blink::Traverser& block_traverser,
	blink::std_traversers::Fudge* fudge_traverser,
	int index,
	int count,
	blink_SamplerDrawInfo* out)
{
	snd::transport::DSPVectorFramePosition sculpted_block_positions;
	snd::transport::DSPVectorFramePosition warped_block_positions;
	ml::DSPVector derivatives;

	fudge_traverser->get_positions(
		data.sliders.speed.value(),
		&data.envelopes.speed.data(),
		data.warp_points,
		block_traverser,
		data.sliders.sample_offset,
		count,
		&sculpted_block_positions,
		&warped_block_positions,
		&derivatives);

	auto sculpted_sample_positions = sculpted_block_positions / (float(song_rate) / sample_data.get_SR());
	auto warped_sample_positions = warped_block_positions / (float(song_rate) / sample_data.get_SR());
	auto final_sample_positions = warped_sample_positions;

	if (data.toggles.loop)
	{
		for (int i = 0; i < count; i++)
		{
			final_sample_positions.set(i, sample_data.get_loop_pos(final_sample_positions[i]));
		}
	}

	if (data.toggles.reverse)
	{
		final_sample_positions = std::int32_t(sample_data.get_num_frames() - 1) - final_sample_positions;
	}

	if (out->sculpted_block_positions)
	{
		const auto doubles = sculpted_block_positions.as_doubles();

		std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_block_positions + index);
	}

	if (out->sculpted_sample_positions)
	{
		const auto doubles = sculpted_sample_positions.as_doubles();

		std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_sample_positions + index);
	}

	if (out->warped_block_positions)
	{
		const auto doubles = warped_block_positions.as_doubles();

		std::copy(doubles[0].data(), doubles[0].data() + count, out->warped_block_positions + index);
	}

	if (out->warped_sample_positions)
	{
		const auto doubles = warped_sample_positions.as_doubles();

		std::copy(doubles[0].data(), doubles[0].data() + count, out->warped_sample_positions + index);
	}

	if (out->final_sample_positions)
	{
		const auto doubles = final_sample_positions.as_doubles();

		std::copy(doubles[0].data(), doubles[0].data() + count, out->final_sample_positions + index);
	}

	if (out->waveform_derivatives)
	{
		std::copy(derivatives.getConstBuffer(), derivatives.getConstBuffer() + count, out->waveform_derivatives + index);
	}
}

static void calculate_amp(const Data& data, const blink::BlockPositions& block_positions, float* out)
{
	auto amp = data.envelopes.amp.search_vec(block_positions) * data.sliders.amp.value();

	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + block_positions.count, out);
}

blink_Error GUI::draw(const Plugin* plugin, const blink_SamplerBuffer* buffer, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	block_traverser_.set_reset(0);

	Data data(plugin, buffer);

	const auto sample_data { SampleData { buffer->sample_info, buffer->channel_mode } };

	auto frames_remaining = n;
	int index = 0;

	BlockPositions block_positions;

	while (frames_remaining > 0 && frames_remaining <= buffer->sample_info->num_frames)
	{
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		block_positions(buffer->positions + index, buffer->data_offset, count);

		block_traverser_.generate(block_positions, count);

		traverser_resetter_.check(&data.envelopes.speed.data(), &block_traverser_);

		calculate_positions(
			data,
			sample_data,
			buffer->song_rate,
			block_traverser_,
			&position_traverser_,
			index,
			count,
			out);

		if (out->amp)
		{
			calculate_amp(data, block_positions, out->amp + index);
		}

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}

}
