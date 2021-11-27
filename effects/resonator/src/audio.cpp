#include "audio.h"
#include "plugin.h"
#include "instance.h"
#include "audio_data.h"
#include "convert.h"

using namespace blink;

namespace resonator {

Audio::Audio(Instance* instance)
	: EffectUnit(instance)
	, plugin_(instance->get_plugin())
	, SR_f_{ float(SR()) }
	, SR_vec_{ SR_f_ }
	, delay_{ SR_f_ }
	, harmonics_{
		Harmonic{SR_f_, {}},
		Harmonic{SR_f_, {}},
		Harmonic{SR_f_, {}} }
{
}

void Audio::stream_init()
{
	SR_f_ = float(SR());
	delay_ = snd::audio::FeedbackDelay<2> { SR_f_ };

	for (auto& harmonic : harmonics_)
	{
		harmonic.delay = snd::audio::FeedbackDelay<2> { SR_f_ };
		harmonic.filter = snd::audio::filter::Filter_1Pole<2>{};
	}

	SR_vec_ = ml::DSPVector(SR_f_);
}

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	static constexpr auto EPSILON { 0.0000001f };

	AudioData data(plugin_, buffer);

	struct
	{
		ml::DSPVector pitch;
		ml::DSPVector feedback;
		ml::DSPVector damper;

		struct
		{
			ml::DSPVector amount;
			ml::DSPVector ratio;
		} fm;

		struct
		{
			ml::DSPVectorInt scale;
			ml::DSPVector amount;
			ml::DSPVector spread;
			ml::DSPVector scale_snap_amount;
		} harmonics;

		ml::DSPVector width;
		ml::DSPVector mix;
	} params;

	params.pitch = data.envelopes.pitch.search_vec(block_positions());
	params.feedback = data.envelopes.feedback.search_vec(block_positions());
	params.damper = data.envelopes.damper.search_vec(block_positions());
	params.fm.amount = data.envelopes.fm_amount.search_vec(block_positions());
	params.fm.ratio = data.envelopes.fm_ratio.search_vec(block_positions());
	params.harmonics.scale = data.chords.harmonics_scale.search_vec(block_positions());
	params.harmonics.amount = data.envelopes.harmonics_amount.search_vec(block_positions());
	params.harmonics.spread = data.envelopes.harmonics_spread.search_vec(block_positions());
	params.harmonics.scale_snap_amount = data.envelopes.harmonics_scale_snap_amount.search_vec(block_positions());
	params.width = data.envelopes.width.search_vec(block_positions());
	params.mix = data.envelopes.mix.search_vec(block_positions());

	const auto damper_mix = ml::repeatRows<2>(params.damper);
	const auto damper_freq = ml::repeatRows<2>(math::convert::linear_to_filter_hz<5, 30000>(1.0f - math::ease::quadratic::in(params.damper)));

	const auto base_pitch = params.pitch + 60.0f;
	const auto base_frequency = math::convert::pitch_to_frequency(base_pitch);

	const auto fm_amount_curve = [](const ml::DSPVector& x) { return x*x*x*x*x*x; };

	const auto fm_amount = fm_amount_curve(params.fm.amount);
	const auto fm_ratio = convert::linear_to_ratio(params.fm.ratio);
	const auto fm_freq = base_frequency * fm_ratio;
	const auto fm = sine_(fm_freq / SR_vec_);
	const auto pitch = base_pitch + (fm * fm_amount * 60.0f);
	const auto frequency = math::convert::pitch_to_frequency(pitch);
	const auto feedback = math::ease::exponential::out(ml::repeatRows<2>(params.feedback));

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	const auto dampener = [this, &damper_mix, &damper_freq](const ml::DSPVectorArray<2>& x)
	{
		filter_(x, SR(), damper_freq);

		return ml::lerp(x, filter_.lp(), damper_mix);
	};
	
	const auto clamp01 = [](const ml::DSPVector& x)
	{
		static const ml::DSPVector ZERO { 0.0f };
		static const ml::DSPVector ONE { 1.0f };

		return ml::clamp(x, ZERO, ONE);
	};

	const auto pan_harmonic = [clamp01](const ml::DSPVectorArray<2>& dry, const ml::DSPVector& pan)
	{
		const auto LL = clamp01(ml::DSPVector{1.0f} - pan); // Amount of L to contribute to L
		const auto LR = clamp01(ml::DSPVector{0.0f} - pan); // Amount of R to contribute to L
		const auto RL = clamp01(pan);          // Amount of L to contribute to R
		const auto RR = clamp01(pan + 1.0f);   // Amount of R to contribute to R

		ml::DSPVectorArray<2> out;

		out.row(0) = LL * dry.constRow(0) + LR * dry.constRow(1);
		out.row(1) = RL * dry.constRow(0) + RR * dry.constRow(1);

		return out;
	};

	const auto get_harmonic_pan_vector = [](int index = -1)
	{
		switch (index)
		{
			default: return -0.5f;
			case 0: return 0.5f;
			case 1: return -1.0f;
			case 2: return 1.0f;
		}
	};

	const auto get_harmonic_pan = [clamp01, get_harmonic_pan_vector, params](int index = -1)
	{
		return get_harmonic_pan_vector(index) * params.width * clamp01(params.harmonics.amount);
	};

	auto delay_samples = ml::repeatRows<2>(SR_vec_ / frequency);

	out_vec = delay_(in_vec, delay_samples, feedback, dampener);
	out_vec = pan_harmonic(out_vec, get_harmonic_pan());

	const auto get_harmonic_ratio = [&params](int harmonic)
	{
		const auto ff = 1.0f + (float(harmonic + 1) * params.harmonics.spread);
		const auto harmonic_pitch = blink::math::convert::ff_to_p(ff);
		const auto snapped_pitch = blink::ChordParameter::snap_pitch_to_scale(harmonic_pitch, params.harmonics.scale);
		const auto pitch = ml::lerp(harmonic_pitch, snapped_pitch, params.harmonics.scale_snap_amount);

		return blink::math::convert::p_to_ff(pitch);
	};

	ml::DSPVectorArray<2> amp { 1.0f };

	for (int i = 0; i < harmonics_.size(); i++)
	{
		const auto harmonic_amp_v = ml::clamp(params.harmonics.amount - float(i), {0.0f}, {1.0f});

		if (ml::max(harmonic_amp_v) > EPSILON)
		{
			auto& filter = harmonics_[i].filter;

			const auto dampener = [this, &filter, &damper_mix, &damper_freq](const ml::DSPVectorArray<2>& x)
			{
				filter(x, SR(), damper_freq);

				return ml::lerp(x, filter.lp(), damper_mix);
			};

			const auto harmonic_amp = ml::repeatRows<2>(harmonic_amp_v);
			const auto ratio = get_harmonic_ratio(i);
			const auto harmonic_frequency = frequency * ratio;
			const auto harmonic_delay_samples = ml::repeatRows<2>(SR_vec_ / harmonic_frequency);
			auto harmonic_out = harmonics_[i].delay(in_vec, harmonic_delay_samples, feedback, dampener);

			harmonic_out = pan_harmonic(harmonic_out, get_harmonic_pan(i));

			out_vec += harmonic_out * harmonic_amp;
			amp += harmonic_amp;
		}
		else
		{
			harmonics_[i].delay.clear();
			harmonics_[i].filter.clear();
		}
	}

	out_vec /= amp;

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(params.mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	sine_.clear();
	delay_.clear();
	filter_ = snd::audio::filter::Filter_1Pole<2>{};

	for (auto& harmonic: harmonics_)
	{
		harmonic.delay.clear();
		harmonic.filter = snd::audio::filter::Filter_1Pole<2>{};
	}
}

} // resonator
