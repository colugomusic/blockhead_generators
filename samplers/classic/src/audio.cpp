#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"
#include <blink/dsp.hpp>

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

	block_traverser_.generate(block_positions(), kFloatsPerDSPVector);

	const auto warp_points = unit_state.warp_points;

	traverser_resetter_.check(&data.envelopes.pitch.data(), &block_traverser_);

	snd::transport::DSPVectorFramePosition sample_pos;

	position_traverser_.get_positions(
		data.sliders.pitch.value(),
		&data.envelopes.pitch.data(),
		warp_points,
		block_traverser_,
		data.sliders.sample_offset,
		kFloatsPerDSPVector,
		nullptr,
		&sample_pos,
		nullptr);

	sample_pos /= float(buffer.song_rate) / buffer.sample_info->SR;

	SampleData sample_data(buffer.sample_info, unit_state.channel_mode);

	auto amp = data.envelopes.amp.search_vec(block_positions()) * data.sliders.amp.value();

	if (data.toggles.reverse)
	{
		sample_pos = std::int32_t(buffer.sample_info->num_frames - 1) - sample_pos;
	}

	if (buffer.sample_info->num_channels > 1)
	{
		out_vec = process_stereo_sample(sample_data, sample_pos, data.toggles.loop);
	}
	else
	{
		out_vec = process_mono_sample(sample_data, sample_pos, data.toggles.loop);
	}

	out_vec =
		noise_gen_(
			out_vec,
			data.options.noise_mode,
			data.envelopes.noise_amount,
			data.envelopes.noise_color,
			data.sliders.noise_width.value(),
			block_positions());
	
	out_vec = stereo_pan(out_vec, data.sliders.pan.value(), data.envelopes.pan, block_positions());
	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
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

} // classic