#include "gui.h"
#include "plugin.h"
#include <blink/envelope_data.hpp>
#include <blink/sample_data.hpp>
#include <blink/slider_data.hpp>
#include <blink/parameters/option_data.hpp>

using namespace blink;

namespace fudge {

struct Data
{
	struct Sliders
	{
		Sliders(const Plugin& plugin, const blink_ParameterData* parameter_data)
			: amp(plugin.params.sliders.amp->slider, parameter_data)
			, speed(plugin.params.sliders.speed->slider, parameter_data)
			, sample_offset(plugin.params.sliders.sample_offset->slider, parameter_data)
		{
		}

		blink::SliderData<int(Parameters::Index::Sld_Amp)> amp;
		blink::SliderData<int(Parameters::Index::Sld_Speed)> speed;
		blink::IntSliderData<int(Parameters::Index::Sld_SampleOffset)> sample_offset;
	} sliders;

	struct Toggles
	{
		Toggles(const Plugin& plugin, const blink_ParameterData* parameter_data)
			: loop { *plugin.params.toggles.loop, parameter_data }
			, reverse { *plugin.params.toggles.reverse, parameter_data }
		{
		}

		blink::OptionData<int(Parameters::Index::Tog_Loop)> loop;
		blink::OptionData<int(Parameters::Index::Tog_Reverse)> reverse;
	} toggles;

	struct Envelopes
	{
		Envelopes(const Plugin& plugin, const blink_ParameterData* parameter_data)
			: amp(plugin.params.env.amp->envelope, parameter_data)
			, speed(plugin.params.env.speed->envelope, parameter_data)
		{
		}

		blink::EnvelopeData<int(Parameters::Index::Env_Amp)> amp;
		blink::EnvelopeData<int(Parameters::Index::Env_Speed)> speed;
	} envelopes;

	struct Options
	{
		Options(const Plugin& plugin, const blink_ParameterData* parameter_data)
			: reverse { *plugin.params.options.reverse_mode.get(), parameter_data }
		{
		}

		blink::OptionData<int(Parameters::Index::Option_ReverseMode)> reverse;
	} options;

	const blink_WarpPoints* warp_points;

	Data(const Plugin& plugin, const blink_SamplerUnitState& unit_state)
		: sliders(plugin, unit_state.parameter_data)
		, toggles(plugin, unit_state.parameter_data)
		, envelopes(plugin, unit_state.parameter_data)
		, options { plugin, unit_state.parameter_data }
		, warp_points(unit_state.warp_points)
	{
	}
};

static void calculate_amp(const Data& data, const blink::BlockPositions& block_positions, float* out)
{
	auto amp = data.envelopes.amp.search_vec(block_positions) * data.sliders.amp.value;

	std::copy(amp.getConstBuffer(), amp.getConstBuffer() + block_positions.count, out);
}

blink_Error GUI::draw(const Plugin& plugin, const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, blink_FrameCount n, blink_SamplerDrawInfo* out)
{
	Data data(plugin, unit_state);

	const auto sample_data { SampleData { buffer.sample_info, unit_state.channel_mode } };

	auto frames_remaining { int64_t(n) };
	int index = 0;

	BlockPositions block_positions;

	while (frames_remaining > 0)
	{
		auto count = std::min(kFloatsPerDSPVector, int(frames_remaining));

		block_positions(buffer.positions + index, count);

		transform::Stretch::Config config;

		config.unit_state_id = unit_state.id;
		config.env.speed = data.envelopes.speed.data;
		config.option.reverse = data.options.reverse.data;
		config.sample_offset = data.sliders.sample_offset.value;
		config.speed = data.sliders.speed.value;
		config.warp_points = data.warp_points;
		config.outputs.derivatives.sped = true;
		config.outputs.derivatives.warped = true;

		stretch_transformer_(config, block_positions, count);

		auto sculpted_sample_positions { stretch_transformer_.get_sped_positions().positions / (float(buffer.song_rate) / sample_data.get_SR()) };
		auto warped_sample_positions { stretch_transformer_.get_warped_positions().positions / (float(buffer.song_rate) / sample_data.get_SR()) };
		auto final_sample_positions { stretch_transformer_.get_reversed_positions().positions / (float(buffer.song_rate) / sample_data.get_SR()) };

		if (data.toggles.loop.value)
		{
			for (int i = 0; i < count; i++)
			{
				final_sample_positions.set(i, sample_data.get_loop_pos(final_sample_positions[i]));
			}
		}

		if (data.toggles.reverse.value)
		{
			final_sample_positions = std::int32_t(sample_data.get_num_frames() - 1) - final_sample_positions;
		}

		if (out->sculpted_block_positions)
		{
			const auto doubles = stretch_transformer_.get_sped_positions().positions.as_doubles();

			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_block_positions + index);
		}

		if (out->sculpted_sample_positions)
		{
			const auto doubles = sculpted_sample_positions.as_doubles();

			std::copy(doubles[0].data(), doubles[0].data() + count, out->sculpted_sample_positions + index);
		}

		if (out->warped_block_positions)
		{
			const auto doubles = stretch_transformer_.get_warped_positions().positions.as_doubles();

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
			const auto& sped_derivatives { stretch_transformer_.get_sped_derivatives() };
			const auto& warped_derivatives { stretch_transformer_.get_warped_derivatives() };
			const auto derivatives { sped_derivatives * warped_derivatives };

			std::copy(derivatives.getConstBuffer(), derivatives.getConstBuffer() + count, out->waveform_derivatives + index);
		}

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
