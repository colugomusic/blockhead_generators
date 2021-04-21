#include "audio.h"
#include "plugin.h"
#include "audio_data.h"

using namespace blink;

Audio::Audio(const Wavetable* plugin)
	: plugin_(plugin)
{
}

blink_Error Audio::process(const blink_SamplerBuffer* buffer, float* out)
{
	begin_process(buffer);

	ml::DSPVectorArray<2> out_vec;

	block_traverser_.generate(block_positions(), kFloatsPerDSPVector);

	AudioData data;

	data.option_noise_mode     = plugin_->get_option_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Option_NoiseMode));
	data.env_amp               = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Amp));
	data.env_pan               = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Pan));
	data.env_pitch             = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Pitch));
	data.env_speed             = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_Speed));
	data.env_noise_amount      = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_NoiseAmount));
	data.env_noise_color       = plugin_->get_envelope_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Env_NoiseColor));
	data.slider_amp            = plugin_->get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Amp));
	data.slider_pan            = plugin_->get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Pan));
	data.slider_pitch          = plugin_->get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Pitch));
	data.slider_speed          = plugin_->get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_Speed));
	data.slider_sample_offset  = plugin_->get_int_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_SampleOffset));
	data.slider_noise_width    = plugin_->get_slider_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Sld_NoiseWidth));
	data.toggle_loop           = plugin_->get_toggle_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Tog_Loop));
	data.toggle_reverse        = plugin_->get_toggle_data(buffer->parameter_data, int(Wavetable::ParameterIndex::Tog_Reverse));
	data.warp_points           = buffer->warp_points;

	traverser_resetter_.check(data.env_speed, &block_traverser_);

	const auto analysis_data = buffer->sample_info->analysis_ready ? plugin_->get_analysis_data(buffer->sample_info->id) : nullptr;
	const auto pitch = plugin_->env_pitch().search_vec(data.env_pitch, block_positions()) + 60.0f + data.slider_pitch->value;
	const auto ff = blink::math::convert::pitch_to_frequency(pitch);

	if (analysis_data)
	{
		ml::DSPVector sample_positions;

		position_traverser_.get_positions(
			data.slider_speed->value,
			data.env_speed,
			data.warp_points,
			block_traverser_,
			data.slider_sample_offset->value,
			kFloatsPerDSPVector,
			&sample_positions);

		struct Wavecycle
		{
			std::uint32_t beg;
			std::uint32_t end;
			float midpoint;
		};

		const auto phase = phasor_(ff / float(buffer->sample_rate));

		ml::DSPVector in_pos_0;
		ml::DSPVector in_pos_1;
		ml::DSPVector out_pos_0;
		ml::DSPVector out_pos_1;
		ml::DSPVector xfade;

		for (int i = 0; i < kFloatsPerDSPVector; i++)
		{
			const auto sample_position = std::clamp(sample_positions[i], float(0), float(buffer->sample_info->num_frames - 1));
			const auto frame_analysis = analysis_data->data[0][std::size_t(std::floor(sample_position))];
			
			Wavecycle this_wavecycle;

			this_wavecycle.beg = frame_analysis.prev_crossing;
			this_wavecycle.end = frame_analysis.next_crossing;
			this_wavecycle.midpoint = blink::math::lerp(float(this_wavecycle.beg), float(this_wavecycle.end), 0.5f);

			if (sample_position < this_wavecycle.midpoint)
			{
				Wavecycle prev_wavecycle;

				prev_wavecycle.beg = analysis_data->data[0][this_wavecycle.beg == 0 ? 0 : this_wavecycle.beg - 1].prev_crossing;
				prev_wavecycle.end = this_wavecycle.beg;
				prev_wavecycle.midpoint = blink::math::lerp(float(prev_wavecycle.beg), float(prev_wavecycle.end), 0.5f);

				in_pos_0[i] = blink::math::lerp(float(prev_wavecycle.beg), prev_wavecycle.midpoint, phase[i]);
				in_pos_1[i] = blink::math::lerp(float(this_wavecycle.beg), this_wavecycle.midpoint, phase[i]);
				out_pos_0[i] = blink::math::lerp(prev_wavecycle.midpoint, float(prev_wavecycle.end), phase[i]);
				out_pos_1[i] = blink::math::lerp(this_wavecycle.midpoint, float(this_wavecycle.end), phase[i]);
				xfade[i] = blink::math::inverse_lerp(prev_wavecycle.midpoint, this_wavecycle.midpoint, sample_position);
			}
			else
			{
				Wavecycle next_wavecycle;

				next_wavecycle.beg = this_wavecycle.end;
				next_wavecycle.end = analysis_data->data[0][std::min(this_wavecycle.end, buffer->sample_info->num_frames - 1)].next_crossing;
				next_wavecycle.midpoint = blink::math::lerp(float(next_wavecycle.beg), float(next_wavecycle.end), 0.5f);

				in_pos_0[i] = blink::math::lerp(float(this_wavecycle.beg), this_wavecycle.midpoint, phase[i]);
				in_pos_1[i] = blink::math::lerp(float(next_wavecycle.beg), next_wavecycle.midpoint, phase[i]);
				out_pos_0[i] = blink::math::lerp(this_wavecycle.midpoint, float(this_wavecycle.end), phase[i]);
				out_pos_1[i] = blink::math::lerp(next_wavecycle.midpoint, float(next_wavecycle.end), phase[i]);
				xfade[i] = blink::math::inverse_lerp(this_wavecycle.midpoint, next_wavecycle.midpoint, sample_position);
			}
		}

		blink::SampleData sample_data(buffer->sample_info, buffer->channel_mode);

		const auto fade = blink::math::ease::parametric::in_out(phase);

		auto grain0_in = sample_data.read_frames_interp<1>(in_pos_0, data.toggle_loop->value) * fade;
		auto grain0_out = sample_data.read_frames_interp<1>(out_pos_0, data.toggle_loop->value) * (1.0f - fade);
		auto grain1_in = sample_data.read_frames_interp<1>(in_pos_1, data.toggle_loop->value) * fade;
		auto grain1_out = sample_data.read_frames_interp<1>(out_pos_1, data.toggle_loop->value) * (1.0f - fade);

		out_vec = ml::repeatRows<2>(ml::lerp(grain0_in + grain0_out, grain1_in + grain1_out, xfade));
	}

	out_vec = add_noise(out_vec, data.option_noise_mode->index, data.env_noise_amount, data.env_noise_color, data.slider_noise_width, block_positions());
	out_vec = stereo_pan(out_vec, data.slider_pan->value, plugin_->env_pan(), data.env_pan, block_positions());

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions()) * data.slider_amp->value;

	out_vec *= ml::repeatRows<2>(amp);

	ml::storeAligned(out_vec.constRow(0), out);
	ml::storeAligned(out_vec.constRow(1), out + kFloatsPerDSPVector);

	return BLINK_OK;
}

ml::DSPVectorArray<2> Audio::add_noise(
	const ml::DSPVectorArray<2>& in,
	int mode,
	const blink_EnvelopeData* env_noise_amount,
	const blink_EnvelopeData* env_noise_color,
	const blink_SliderData* sld_noise_width,
	const blink::BlockPositions& block_positions)
{
	ml::DSPVectorArray<2> out;

	if (env_noise_amount->points.count < 1)
	{
		return in;
	}

	const auto noise_amount = plugin_->env_noise_amount().search_vec(env_noise_amount, block_positions);

	if (ml::sum(noise_amount) < 0.0001f)
	{
		return in;
	}

	float noise_color;

	plugin_->env_noise_color().search_vec(env_noise_color, block_positions.positions.getConstBuffer(), 1, block_positions.prev_pos, &noise_color);

	noise_filter_.mCoeffs = ml::OnePole::coeffs(0.001f + (std::pow(noise_color, 2.0f) * 0.6f));

	constexpr auto MULTIPLY = 0;
	constexpr auto MIX = 1;

	const auto noise_L = noise_filter_(noise_gen_());

	ml::DSPVectorArray<2> noise;

	if (sld_noise_width->value > 0.0f)
	{
		const auto noise_R = noise_filter_(noise_gen_());

		noise = ml::concatRows(noise_L, ml::lerp(noise_L, noise_R, sld_noise_width->value));
	}
	else
	{
		noise = ml::repeatRows<2>(noise_L);
	}

	if (mode == MULTIPLY)
	{
		return in * ml::lerp(ml::DSPVectorArray<2>(1.0f), noise, ml::repeatRows<2>(noise_amount));
	}
	else
	{
		return ml::lerp(in, noise, ml::repeatRows<2>(noise_amount));
	}
}

