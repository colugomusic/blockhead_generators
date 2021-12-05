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
	, fundamental_{SR_f_, {}}
	, harmonics_{ Resonator{SR_f_, {}}, Resonator{SR_f_, {}}, Resonator{SR_f_, {}} }
{
}

void Audio::stream_init()
{
	SR_f_ = float(SR());

	fundamental_ = { SR_f_, {}};

	for (auto& harmonic : harmonics_)
	{
		harmonic = { SR_f_, {} };
	}

	SR_vec_ = ml::DSPVector(SR_f_);
}

ml::DSPVectorArray<2> Audio::Resonator::dampener(
	const ml::DSPVectorArray<2>& dry,
	blink_SR SR,
	const ml::DSPVectorArray<2>& freq,
	const ml::DSPVectorArray<2>& mix)
{
	filter(dry, SR, freq);

	return ml::lerp(dry, filter.lp(), mix);
}

struct Params
{
	Params(const AudioData& data, const BlockPositions& block_positions)
	{
		env.pitch = data.envelopes.pitch.search_vec(block_positions);
		env.feedback = data.envelopes.feedback.search_vec(block_positions);
		env.damper = data.envelopes.damper.search_vec(block_positions);
		env.fm.amount = data.envelopes.fm_amount.search_vec(block_positions);
		env.fm.ratio = data.envelopes.fm_ratio.search_vec(block_positions);
		env.harmonics.amount = data.envelopes.harmonics_amount.search_vec(block_positions);
		env.harmonics.spread = data.envelopes.harmonics_spread.search_vec(block_positions);
		env.harmonics.scale_snap_amount = data.envelopes.harmonics_scale_snap_amount.search_vec(block_positions);
		env.width = data.envelopes.width.search_vec(block_positions);
		env.mix = data.envelopes.mix.search_vec(block_positions);

		chord.harmonics.scale = data.chords.harmonics_scale.search_vec(block_positions);

		slider.pitch = data.sliders.pitch;
	}

	struct
	{
		ml::DSPVector pitch, feedback, damper, width, mix;

		struct { ml::DSPVector amount, ratio; } fm;
		struct { ml::DSPVector amount, spread, scale_snap_amount, width; } harmonics;
	} env;

	struct
	{
		struct { ml::DSPVectorInt scale; } harmonics;
	} chord;

	struct
	{
		float pitch;
	} slider;
};

blink_Error Audio::process(const blink_EffectBuffer* buffer, const float* in, float* out)
{
	static constexpr auto EPSILON { 0.0000001f };

	AudioData data(plugin_, buffer);
	Params params(data, block_positions());

	const auto damper_mix = ml::repeatRows<2>(params.env.damper);
	const auto damper_freq = ml::repeatRows<2>(math::convert::linear_to_filter_hz<5, 30000>(1.0f - math::ease::quadratic::in(params.env.damper)));
	const auto base_pitch = params.env.pitch + params.slider.pitch + 60.0f;
	const auto base_frequency = math::convert::pitch_to_frequency(base_pitch);
	const auto fm_amount_curve = [](const ml::DSPVector& x) { return x*x*x*x*x*x; };
	const auto fm_amount = fm_amount_curve(params.env.fm.amount);
	const auto fm_ratio = convert::linear_to_ratio(params.env.fm.ratio);
	const auto fm_freq = base_frequency * fm_ratio;
	const auto fm = fm_source_(fm_freq / SR_vec_);
	const auto pitch = base_pitch + (fm * fm_amount * 60.0f);
	const auto frequency = ml::repeatRows<2>(math::convert::pitch_to_frequency(pitch));
	const auto feedback = math::ease::exponential::out(ml::repeatRows<2>(params.env.feedback));

	ml::DSPVectorArray<2> in_vec(in);
	ml::DSPVectorArray<2> out_vec;

	const auto run_resonator = [this](
		Resonator* resonator,
		const ml::DSPVectorArray<2>& freq,
		const ml::DSPVectorArray<2>& feedback,
		const ml::DSPVectorArray<2>& damper_freq,
		const ml::DSPVectorArray<2>& damper_mix,
		const ml::DSPVectorArray<2>& dry)
	{
		const auto dampener = [this, resonator, &damper_mix, &damper_freq](const ml::DSPVectorArray<2>& dry)
		{
			return resonator->dampener(dry, SR(), damper_freq, damper_mix);
		};

		auto delay_samples = ml::repeatRows<2>(SR_vec_) / freq;

		return resonator->delay(dry, delay_samples, feedback, dampener);
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

	const auto get_harmonic_pan = [clamp01, params](int index)
	{
		return PAN_VECTORS[index] * params.env.width * clamp01(params.env.harmonics.amount);
	};

	out_vec = run_resonator(&fundamental_, frequency, feedback, damper_freq, damper_mix, in_vec);
	out_vec = pan_harmonic(out_vec, get_harmonic_pan(0));

	const auto get_harmonic_ratio = [&params](int harmonic)
	{
		const auto ff = 1.0f + (float(harmonic + 1) * params.env.harmonics.spread);
		const auto harmonic_pitch = blink::math::convert::ff_to_p(ff);
		const auto snapped_pitch = blink::ChordParameter::snap_pitch_to_scale(harmonic_pitch, params.chord.harmonics.scale);
		const auto pitch = ml::lerp(harmonic_pitch, snapped_pitch, params.env.harmonics.scale_snap_amount);

		return blink::math::convert::p_to_ff(pitch);
	};

	ml::DSPVectorArray<2> amp { 1.0f };

	for (int i = 0; i < harmonics_.size(); i++)
	{
		const auto harmonic_amp_v = ml::clamp(params.env.harmonics.amount - float(i), {0.0f}, {1.0f});

		if (ml::max(harmonic_amp_v) > EPSILON)
		{
			auto& filter = harmonics_[i].filter;

			const auto harmonic_amp = ml::repeatRows<2>(harmonic_amp_v);
			const auto ratio = get_harmonic_ratio(i);
			const auto harmonic_frequency = frequency * ml::repeatRows<2>(ratio);

			auto harmonic_out = run_resonator(&harmonics_[i], harmonic_frequency, feedback, damper_freq, damper_mix, in_vec);

			harmonic_out = pan_harmonic(harmonic_out, get_harmonic_pan(i + 1));

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

 	out_vec = ml::lerp(in_vec, out_vec, ml::repeatRows<2>(params.env.mix));

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

void Audio::reset()
{
	fm_source_.clear();

	fundamental_.delay.clear();
	fundamental_.filter = {};

	for (auto& harmonic: harmonics_)
	{
		harmonic.delay.clear();
		harmonic.filter = {};
	}
}

} // resonator
