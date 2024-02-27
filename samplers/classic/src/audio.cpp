#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include <blink/dsp.hpp>
#include <snd/ease.hpp>

using namespace blink;

namespace classic {

Audio::Audio(Instance* instance)
	: SamplerUnit(instance)
	, plugin_(instance->get_plugin())
{
}

blink_Error Audio::process(const blink_SamplerBuffer& buffer, const blink_SamplerUnitState& unit_state, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	AudioData data(*plugin_, unit_state.parameter_data);

	const auto generate_correction_grains {
		unit_state.smooth_transitions &&
		data.options.reverse_mode.data->points.count > 1
	};
		
	transform::Tape::Config config;

	config.unit_state_id = unit_state.id;
	config.env.pitch = data.envelopes.pitch.data;
	config.option.reverse = data.options.reverse_mode.data;
	config.sample_offset = data.sliders.sample_offset.value;
	config.transpose = data.sliders.pitch.value;
	config.warp_points = unit_state.warp_points;
	config.outputs.derivatives.pitch = false;
	config.outputs.derivatives.warped = false;
	config.outputs.correction_grains = generate_correction_grains;

	tape_transformer_.xform(config, block_positions(), kFloatsPerDSPVector);

	auto sample_pos { tape_transformer_.get_reversed_positions().positions };

	if (generate_correction_grains) {
		reverse_correction_.dry_positions(sample_pos, kFloatsPerDSPVector);
	}

	SampleData sample_data(buffer.sample_info, unit_state.channel_mode);

	out_vec = process_sample(data, buffer, sample_data, sample_pos);

	if (generate_correction_grains)
	{
		out_vec = apply_correction_grains(
			out_vec, 
			data, 
			buffer, 
			sample_data, 
			tape_transformer_.get_correction_grains());
	}

	out_vec =
		noise_gen_(
			out_vec,
			float(data.options.noise_mode.value),
			data.envelopes.noise_amount,
			data.envelopes.noise_color,
			data.sliders.noise_width.search(block_positions()),
			block_positions());
	
	auto amp { data.envelopes.amp.search_vec(block_positions()) * data.sliders.amp.value };

	out_vec = stereo_pan(out_vec, data.sliders.pan.value, data.envelopes.pan, block_positions());
	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

ml::DSPVectorArray<2> Audio::apply_correction_grains(
	const ml::DSPVectorArray<2>& dry,
	const AudioData& data,
	const blink_SamplerBuffer& buffer,
	const blink::SampleData& sample_data,
	const blink::transform::CorrectionGrains& grain_info)
{
	if (!reverse_correction_.grain.on && grain_info.count < 1) return dry;

	auto grains_remaining { grain_info.count };

	snd::transport::DSPVectorFramePosition grain_positions;
	ml::DSPVector xfade;

	for (int i { 0 }; i < kFloatsPerDSPVector; i++)
	{
		if (reverse_correction_.grain.on)
		{
			// TODO: multiply by scale here???
			reverse_correction_.grain.pos += reverse_correction_.grain.ff;
			reverse_correction_.grain.vpos += reverse_correction_.grain.vff;

			grain_positions.set(i, reverse_correction_.grain.pos);
			xfade[i] = snd::ease::quadratic::in_out(snd::inverse_lerp(reverse_correction_.grain.beg, reverse_correction_.grain.vend, reverse_correction_.grain.vpos));

			if (reverse_correction_.grain.vpos >= reverse_correction_.grain.vend) reverse_correction_.grain.on = false;
		}
		else
		{
			xfade[i] = 1.0f;

			if (grains_remaining > 0 && i == grain_info.buffer_index[grain_info.count - grains_remaining])
			{
				reverse_correction_.grain.on = true;

				const auto beg { i == 0 ? reverse_correction_.dry_positions.prev_pos : reverse_correction_.dry_positions[i - 1] };

				// i hate math
				const auto a { float(buffer.song_rate) / buffer.sample_info->SR };
				const auto b { float(buffer.sample_info->SR) / SR() };

				reverse_correction_.grain.beg = beg;
				reverse_correction_.grain.pos = beg;
				reverse_correction_.grain.ff = grain_info.ff[i] * a * b;
				reverse_correction_.grain.vpos = beg;
				reverse_correction_.grain.vend = beg + grain_info.length[i];
				reverse_correction_.grain.vff = std::abs(grain_info.ff[i]);

				grain_positions.set(i, reverse_correction_.grain.pos);
				xfade[i] = snd::ease::quadratic::in_out(snd::inverse_lerp(reverse_correction_.grain.beg, reverse_correction_.grain.vend, reverse_correction_.grain.vpos));

				grains_remaining--;
			}
		}
	}

	ml::DSPVectorArray<2> out;

	const auto wet { process_sample(data, buffer, sample_data, grain_positions) };

	return ml::lerp(wet, dry, ml::repeatRows<2>(xfade));
}

ml::DSPVectorArray<2> Audio::process_sample(
	const AudioData& data,
	const blink_SamplerBuffer& buffer,
	const blink::SampleData& sample_data,
	snd::transport::DSPVectorFramePosition sample_pos)
{
	sample_pos /= float(buffer.song_rate) / buffer.sample_info->SR;

	if (data.toggles.reverse.value)
	{
		sample_pos = std::int32_t(buffer.sample_info->num_frames - 1) - sample_pos;
	}

	if (buffer.sample_info->num_channels > 1)
	{
		return process_stereo_sample(sample_data, sample_pos, data.toggles.loop.value);
	}
	else
	{
		return process_mono_sample(sample_data, sample_pos, data.toggles.loop.value);
	}
}

ml::DSPVectorArray<2> Audio::process_stereo_sample(const SampleData& sample_data, const snd::transport::DSPVectorFramePosition &sample_pos, bool loop)
{
	ml::DSPVectorArray<2> out;

	switch (sample_data.get_channel_mode())
	{
		default:
		case blink_ChannelMode_Stereo:
		{
			return sample_data.read_frames_interp<2>(sample_pos, loop);
		}

		case blink_ChannelMode_StereoSwap:
		{
			return ml::rotateRows(sample_data.read_frames_interp<2>(sample_pos, loop), 1);
		}

		case blink_ChannelMode_Left:
		{
			return ml::repeatRows<2>(sample_data.read_frames_interp(0, sample_pos, loop));
		}

		case blink_ChannelMode_Right:
		{
			return ml::repeatRows<2>(sample_data.read_frames_interp(1, sample_pos, loop));
		}
	}
}

ml::DSPVectorArray<2> Audio::process_mono_sample(const SampleData& sample_data, const snd::transport::DSPVectorFramePosition& sample_pos, bool loop)
{
	return ml::repeatRows<2>(sample_data.read_frames_interp(0, sample_pos, loop));
}

blink_Error Audio::preprocess_sample(void* host, blink_PreprocessCallbacks callbacks) const
{
	return BLINK_OK;
}

void Audio::reset()
{
	reverse_correction_.grain = {};
}

} // classic