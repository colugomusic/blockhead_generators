#include "gui.h"
#include "plugin.h"
#include <blink/envelope_data.hpp>
#include <blink/sample_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/parameters/toggle_data.hpp>

using namespace blink;

namespace classic {

struct Data
{
	struct Sliders
	{
		Sliders(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: amp(plugin, plugin->params().sliders.amp->slider(), buffer->parameter_data)
			, pitch(plugin, plugin->params().sliders.pitch->slider(), buffer->parameter_data)
			, sample_offset(plugin->get_int_slider_data(buffer->parameter_data, int(Parameters::Index::Sld_SampleOffset))->value)
		{
		}

		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Pitch)> pitch;

		int sample_offset;
	} sliders;

	struct Toggles
	{
		Toggles(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: loop(plugin, *plugin->params().toggles.loop.get(), buffer->parameter_data)
			, reverse(plugin, *plugin->params().toggles.reverse.get(), buffer->parameter_data)
		{
		}

		blink::ToggleData<int(Parameters::Index::Tog_Loop)> loop;
		blink::ToggleData<int(Parameters::Index::Tog_Reverse)> reverse;
	} toggles;

	struct Envelopes
	{
		Envelopes(const Plugin* plugin, const blink_SamplerBuffer* buffer)
			: amp(plugin, plugin->params().env.amp->envelope(), buffer->parameter_data)
			, pitch(plugin, plugin->params().env.pitch->envelope(), buffer->parameter_data)
		{
		}

		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Pitch)> pitch;
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
	blink::std_traversers::Classic* classic_traverser,
	int index,
	int count,
	blink_SamplerDrawInfo* out)
{
	snd::transport::DSPVectorFramePosition sculpted_block_positions;
	snd::transport::DSPVectorFramePosition warped_block_positions;
	ml::DSPVector derivatives;

	classic_traverser->get_positions(
		data.sliders.pitch.value(),
		&data.envelopes.pitch.data(),
		data.warp_points,
		block_traverser,
		data.sliders.sample_offset ? data.sliders.sample_offset : 0,
		count,
		&sculpted_block_positions,
		&warped_block_positions,
		&derivatives);

	auto sculpted_sample_positions = sculpted_block_positions / (float(song_rate) / sample_data.get_SR());
	auto warped_sample_positions = warped_block_positions / (float(song_rate) / sample_data.get_SR());
	auto final_sample_positions = warped_sample_positions;

	if (data.toggles.loop.value())
	{
		for (int i = 0; i < count; i++)
		{
			final_sample_positions.set(i, sample_data.get_loop_pos(final_sample_positions[i]));
		}
	}

	if (data.toggles.reverse.value())
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

static void calculate_amp(const Plugin* plugin, const Data& data, const blink::BlockPositions& block_positions, float* out)
{
	auto amp = data.envelopes.amp.search_vec(block_positions);

	amp *= data.sliders.amp.search_vec(block_positions);

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

		traverser_resetter_.check(&data.envelopes.pitch.data(), &block_traverser_);

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
			calculate_amp(plugin, data, block_positions, out->amp + index);
		}

		frames_remaining -= kFloatsPerDSPVector;
		index += kFloatsPerDSPVector;
	}

	return BLINK_OK;
}

} // classic