#include "audio.h"
#include "audio_data.h"
#include "plugin.h"
#include "instance.h"

using namespace blink;

namespace wavebender {

static constexpr auto INIT = 4;

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
{
}

void Audio::stream_init()
{
	for (int c = 0; c < 2; c++)
	{
		for (int i = 0; i < 4; i++)
		{
			channels_[c].buffers[i].resize(SR());
		}
	}

	reset();
}

void Audio::Channel::do_write(const FrameWriteParams& params, float filtered_value, float value)
{
	if (init == 0)
	{
		if(std::abs(value) < 0.000001f) return;

		init++;
	}

	write.span.buffer[write.span.size++] = value;

	if (write.span.size >= buffers[0].size())
	{
		std::swap(write.span, stage.span);
		write.up = false;
		return;
	}

	if (write.up && filtered_value < 0)
	{
		write.up = false;
	}
	else if (!write.up && filtered_value > 0)
	{
		write.up = true;

		if (write.span.size > 3)
		{
			write.counter++;

			if (write.counter > params.bubble)
			{
				if (++init >= INIT)
				{
					std::swap(write.span, stage.span);
				}

				write.span.size = 0;
				write.counter = 0;
			}
		}
	}
}

float Audio::Channel::do_read(const FrameReadParams& params, float in)
{
	if (init == INIT)
	{
		start_fade_in(params);
		init++;
	}

	if (init < INIT) return in;

	float value { do_wet(params) };

	if (fade_in.active)
	{
		const auto amp = math::ease::quadratic::in_out(float(fade_in.index++) / fade_in.length);

		value = math::lerp(in, value, amp);
		
		if (fade_in.index >= fade_in.length)
		{
			fade_in.active = false;
		}
	}

	return value;
}

static float window(float x, float r = 0.5f)
{
	const auto top = 1.0f - r;

	if (x < r)
	{
		return math::ease::parametric::in_out(x * (1.0f / r));
	}
	else if (x > top)
	{
		return 1.0f - math::ease::parametric::in_out((x - top) * (1.0f / r));
	}
	else
	{
		return 1.0f;
	}
}

static float apply_tilt(float frame, float tilt, float spike, size_t size)
{
	const auto pos { frame / size };
	const auto tilted { std::pow(pos, std::pow(2.0f, tilt)) };
	const auto smoothed = math::lerp(pos, tilted, window(pos));

	return math::lerp(smoothed, tilted, spike) * size;
}

void Audio::Channel::prepare_xfade(const FrameReadParams& params)
{
	const auto tmp { target.span };

	target.span = stage.span;
	target.frame = 0.0f;
	stage.span = source.span;
	stage.span.size = 0;
	source.span = tmp;
	source.frame = target.frame;
}

void Audio::Channel::start_fade_in(const FrameReadParams& params)
{
	const auto tmp { target.span };

	target.span = stage.span;
	target.frame = 0.0f;
	stage.span = source.span;
	stage.span.size = 0;
	source.span = tmp;
	source.frame = target.frame;

	fade_in.active = true;
	fade_in.index = 0;
	fade_in.length = target.span.size * 2;
}

void Audio::Channel::start_xfade(const FrameReadParams& params)
{
	xfade.active = true;
	xfade.index = 0;

	if (params.crossfade_mode == CrossfadeMode::Dynamic)
	{
		xfade.length = size_t(float(math::midpoint(source.span.size, target.span.size)) * params.crossfade_size);
	}
	else
	{
		xfade.length = size_t(64.0f * params.crossfade_size);
	}

	xfade.source_speed_1 = float(source.span.size) / float(target.span.size);
	xfade.target_speed_0 = float(target.span.size) / float(source.span.size);
}

float Audio::Channel::do_xfade(const FrameReadParams& params)
{
	const auto x { math::ease::quadratic::in_out(float(xfade.index) / (xfade.length - 1)) };

	auto source_value { source.span.read(apply_tilt(source.frame, params.tilt, params.spike, source.span.size)) };
	auto target_value { target.span.read(apply_tilt(target.frame, params.tilt, params.spike, target.span.size)) };

	const auto value { math::lerp(source_value, target_value, x) };

	const auto source_inc = math::lerp(xfade.source_speed_0, xfade.source_speed_1, x) * params.ff;
	const auto target_inc = math::lerp(xfade.target_speed_0, xfade.target_speed_1, x) * params.ff;

	source.frame += source_inc;
	target.frame += target_inc;

	const auto source_end = (source.span.size - 1);
	const auto target_end = (target.span.size - 1);

	if (source.frame > source_end) source.frame -= source_end;
	if (target.frame > target_end) target.frame -= target_end;

	xfade.index++;

	if (xfade.index >= xfade.length)
	{
		xfade.active = false;
	}

	return value;
}

float Audio::Channel::do_wet(const FrameReadParams& params)
{
	if (xfade.active)
	{
		return do_xfade(params);
	}
	else
	{
		auto value { target.span.read(apply_tilt(target.frame, params.tilt, params.spike, target.span.size)) };

		target.frame += params.ff;

		const auto end = (target.span.size - 1);

		if (target.frame > end)
		{
			target.frame -= end;

			if (stage.span.size > 0)
			{
				prepare_xfade(params);
				start_xfade(params);
			}
		}

		return value;
	}
}

float Audio::Channel::operator()(const FrameWriteParams& write_params, const FrameReadParams& read_params, float in, float filtered_in)
{
	do_write(write_params, filtered_in, in);

	return do_read(read_params, in);
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	auto bubble = data.envelopes.bubble.search_vec(block_positions());
	auto tilt = math::convert::uni_to_bi(data.envelopes.tilt.search_vec(block_positions())) * 8.0f;
	auto pitch = math::convert::p_to_ff(data.envelopes.pitch.search_vec(block_positions()));
	auto crossfade_size = data.envelopes.xfade_size.search_vec(block_positions());
	auto smoother = math::convert::linear_to_filter_hz(1.0f - data.envelopes.smoother.search(block_positions())) / SR();
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	crossfade_size = ml::lerp(ml::DSPVector(1.0f), ml::DSPVector(32.0f), crossfade_size * crossfade_size);

	auto bubble_int = ml::roundFloatToInt(bubble * bubble * 64.0f);
	const auto spike = ml::clamp(ml::abs(tilt * 0.25f) - 1.0f, ml::DSPVector(0.0f), ml::DSPVector(1.0f));

	tilt = ml::clamp(tilt, ml::DSPVector(-4.0f), ml::DSPVector(4.0f));

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;
	ml::DSPVectorArray<2> filtered_input;

	for (int c = 0; c < 2; c++)
	{
		channels_[c].write.filter.mCoeffs = ml::Lopass::coeffs(smoother, 1.0f);
		filtered_input.row(c) = channels_[c].write.filter(in_vec.constRow(c));

		for (int i = 0; i < kFloatsPerDSPVector; i++)
		{
			FrameWriteParams write_params;

			write_params.bubble = bubble_int[i];

			FrameReadParams read_params;

			read_params.crossfade_size = crossfade_size[i];
			read_params.crossfade_mode = CrossfadeMode(data.options.xfade_mode.get());
			read_params.tilt = tilt[i];
			read_params.spike = spike[i];
			read_params.ff = pitch[i];

			out_vec.row(c)[i] = channels_[c](write_params, read_params, in_vec.constRow(c)[i], filtered_input.constRow(c)[i]);
		}
	}

	ml::validate(out_vec.constRow(0));

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	for (int c = 0; c < 2; c++)
	{
		channels_[c].init = 0;
		channels_[c].fade_in.active = false;
		channels_[c].xfade.active = false;
		channels_[c].write.span.buffer = channels_[c].buffers[0].data();
		channels_[c].write.span.size = 0;
		channels_[c].write.counter = 0;
		channels_[c].write.up = false;
		channels_[c].stage.span.buffer = channels_[c].buffers[1].data();
		channels_[c].stage.span.size = 0;
		channels_[c].source.span.buffer = channels_[c].buffers[2].data();
		channels_[c].source.span.size = 0;
		channels_[c].target.span.buffer = channels_[c].buffers[3].data();
		channels_[c].target.span.size = 0;
	}
}

float Audio::Channel::Span::read(float pos) const
{
	const auto index_0 { size_t(std::floor(pos)) };
	const auto index_1 { size_t(std::ceil(pos)) };
	const auto x { pos - index_0 };

	if (index_0 == index_1)
	{
		return buffer[index_0];
	}

	const auto value_0 { buffer[index_0] };
	const auto value_1 { buffer[index_1] };

	return blink::math::lerp(value_0, value_1, x);
}

} // wavebender