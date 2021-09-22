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
	for (int i = 0; i < 4; i++)
	{
		buffers_[i].resize(SR());
	}

	write_.filter.mCoeffs = ml::Lopass::coeffs(200.0f / SR(), 1.0f);

	reset();
}

void Audio::write(const FrameWriteParams& params, float filtered_value, float value)
{
	if (init_ == 0)
	{
		if(std::abs(value) < 0.000001f) return;

		init_++;
	}

	write_.span.buffer[write_.span.size++] = value;

	if (write_.span.size >= buffers_[0].size())
	{
		std::swap(write_.span, stage_.span);
		write_.up = false;
		return;
	}

	if (write_.up && filtered_value < 0)
	{
		write_.up = false;
	}
	else if (!write_.up && filtered_value > 0)
	{
		write_.up = true;

		if (write_.span.size > 3)
		{
			write_.counter++;

			if (write_.counter > params.bubble)
			{
				if (++init_ >= INIT)
				{
					std::swap(write_.span, stage_.span);
				}

				write_.span.size = 0;
				write_.counter = 0;
			}
		}
	}
}

float Audio::read(const FrameReadParams& params, float in)
{
	if (init_ == INIT)
	{
		start_fade_in(params);
		init_++;
	}

	if (init_ < INIT) return in;

	float value { do_wet(params) };

	if (fade_in_.active)
	{
		const auto amp = math::ease::quadratic::in_out(float(fade_in_.index++) / fade_in_.length);

		value = math::lerp(in, value, amp);
		
		if (fade_in_.index >= fade_in_.length)
		{
			fade_in_.active = false;
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

static float apply_tilt(float frame, float tilt, size_t size)
{
	const auto pos { frame / size };
	const auto tilted { std::pow(pos, std::pow(2.0f, tilt)) };

	return math::lerp(pos, tilted, window(pos)) * size;
}

void Audio::prepare_xfade(const FrameReadParams& params)
{
	const auto tmp { target_.span };

	target_.span = stage_.span;
	target_.frame = 0.0f;
	stage_.span = source_.span;
	stage_.span.size = 0;
	source_.span = tmp;
	source_.frame = target_.frame;
}

void Audio::start_fade_in(const FrameReadParams& params)
{
	const auto tmp { target_.span };

	target_.span = stage_.span;
	target_.frame = 0.0f;
	stage_.span = source_.span;
	stage_.span.size = 0;
	source_.span = tmp;
	source_.frame = target_.frame;

	fade_in_.active = true;
	fade_in_.index = 0;
	fade_in_.length = target_.span.size * 2;
}

void Audio::start_xfade(const FrameReadParams& params)
{
	xfade_.active = true;
	xfade_.index = 0;

	if (params.crossfade_mode == CrossfadeMode::Dynamic)
	{
		xfade_.length = size_t(float(math::midpoint(source_.span.size, target_.span.size)) * params.crossfade_size);
	}
	else
	{
		xfade_.length = size_t(64.0f * params.crossfade_size);
	}

	xfade_.source_speed_1 = float(source_.span.size) / float(target_.span.size);
	xfade_.target_speed_0 = float(target_.span.size) / float(source_.span.size);
}

static float grain_window(float x, size_t size)
{
	const auto fade_length = std::min(128.0f, size * 0.5f);

	if (x < size) return 1.0f;
	if (x < size + fade_length) return 1.0f - math::ease::parametric::in_out((x - size) / fade_length);
	return 0.0f;
}

float Audio::do_xfade(const FrameReadParams& params)
{
	const auto x { math::ease::quadratic::in_out(float(xfade_.index) / (xfade_.length - 1)) };

	auto source_value { source_.span.read(apply_tilt(source_.frame, params.tilt, source_.span.size)) };
	auto target_value { target_.span.read(apply_tilt(target_.frame, params.tilt, target_.span.size)) };

	const auto value { math::lerp(source_value, target_value, x) };

	const auto source_inc = math::lerp(xfade_.source_speed_0, xfade_.source_speed_1, x) * params.ff;
	const auto target_inc = math::lerp(xfade_.target_speed_0, xfade_.target_speed_1, x) * params.ff;

	source_.frame += source_inc;
	target_.frame += target_inc;

	const auto source_end = (source_.span.size - 1);
	const auto target_end = (target_.span.size - 1);

	if (source_.frame > source_end) source_.frame -= source_end;
	if (target_.frame > target_end) target_.frame -= target_end;

	xfade_.index++;

	if (xfade_.index >= xfade_.length)
	{
		xfade_.active = false;
	}

	return value;
}

float Audio::do_wet(const FrameReadParams& params)
{
	if (xfade_.active)
	{
		return do_xfade(params);
	}
	else
	{
		auto value { target_.span.read(apply_tilt(target_.frame, params.tilt, target_.span.size)) };

		target_.frame += params.ff;

		const auto end = (target_.span.size - 1);

		if (target_.frame > end)
		{
			target_.frame -= end;

			if (stage_.span.size > 0)
			{
				prepare_xfade(params);
				start_xfade(params);
			}
		}

		return value;
	}
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	AudioData data(plugin_, buffer);

	auto bubble = data.envelopes.bubble.search_vec(block_positions());
	auto tilt = math::convert::uni_to_bi(data.envelopes.tilt.search_vec(block_positions())) * 4.0f;
	auto pitch = math::convert::p_to_ff(data.envelopes.pitch.search_vec(block_positions()));
	auto crossfade_size = data.envelopes.xfade_size.search_vec(block_positions());
	const auto mix = data.envelopes.mix.search_vec(block_positions());

	crossfade_size = ml::lerp(ml::DSPVector(1.0f), ml::DSPVector(32.0f), crossfade_size * crossfade_size);

	auto bubble_int = ml::roundFloatToInt(bubble * bubble * 64.0f);

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	const auto filtered_input { write_.filter(in_vec.constRow(0)) };

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		const auto filtered_value { filtered_input[i] };
		const auto value { in_vec.constRow(0)[i] };

		FrameWriteParams write_params;

		write_params.bubble = bubble_int[i];

		write(write_params, filtered_value, value);

		FrameReadParams read_params;

		read_params.crossfade_size = crossfade_size[i];
		read_params.crossfade_mode = CrossfadeMode(data.options.xfade_mode.get());
		read_params.tilt = tilt[i];
		read_params.ff = pitch[i];

		const auto out_value { read(read_params, value) };

		out_vec.row(0)[i] = out_value;
		out_vec.row(1)[i] = out_value;
	}

	ml::validate(out_vec.constRow(0));

	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	init_ = 0;
	fade_in_.active = false;
	xfade_.active = false;
	write_.span.buffer = buffers_[0].data();
	write_.span.size = 0;
	write_.counter = 0;
	write_.up = false;
	stage_.span.buffer = buffers_[1].data();
	stage_.span.size = 0;
	source_.span.buffer = buffers_[2].data();
	source_.span.size = 0;
	target_.span.buffer = buffers_[3].data();
	target_.span.size = 0;
}

float Audio::Span::read(float pos) const
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