#include "audio.h"
#include "plugin.h"

using namespace blink;

Audio::Audio(const Fudge* plugin)
	: plugin_(plugin)
{
}

blink_Error Audio::process(const blink_SamplerBuffer* buffer, float* out)
{
	ml::DSPVectorArray<2> out_vec;

	const auto prev_pos = block_traverser_.get_last_pos();

	block_traverser_.generate(buffer->positions, kFloatsPerDSPVector, buffer->data_offset);

	const auto& block_positions = block_traverser_.get_read_position();

	struct Data
	{
		const blink_OptionData* option_noise_mode;
		const blink_EnvelopeData* env_amp;
		const blink_EnvelopeData* env_pan;
		const blink_EnvelopeData* env_pitch;
		const blink_EnvelopeData* env_speed;
		const blink_EnvelopeData* env_noise_amount;
		const blink_EnvelopeData* env_noise_color;
		const blink_SliderData* slider_amp;
		const blink_SliderData* slider_pan;
		const blink_SliderData* slider_pitch;
		const blink_SliderData* slider_speed;
		const blink_IntSliderData* slider_sample_offset;
		const blink_SliderData* slider_noise_width;
		const blink_ToggleData* toggle_loop;
		const blink_ToggleData* toggle_reverse;
	} data;

	data.option_noise_mode    = plugin_->get_option_data(buffer->parameter_data, int(Fudge::ParameterIndex::Option_NoiseMode));
	data.env_amp              = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Amp));
	data.env_pan              = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Pan));
	data.env_pitch            = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Pitch));
	data.env_speed            = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_Speed));
	data.env_noise_amount     = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_NoiseAmount));
	data.env_noise_color      = plugin_->get_envelope_data(buffer->parameter_data, int(Fudge::ParameterIndex::Env_NoiseColor));
	data.slider_amp           = plugin_->get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Amp));
	data.slider_pan           = plugin_->get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Pan));
	data.slider_pitch         = plugin_->get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Pitch));
	data.slider_speed         = plugin_->get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_Speed));
	data.slider_sample_offset = plugin_->get_int_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_SampleOffset));
	data.slider_noise_width   = plugin_->get_slider_data(buffer->parameter_data, int(Fudge::ParameterIndex::Sld_NoiseWidth));
	data.toggle_loop          = plugin_->get_toggle_data(buffer->parameter_data, int(Fudge::ParameterIndex::Tog_Loop));
	data.toggle_reverse       = plugin_->get_toggle_data(buffer->parameter_data, int(Fudge::ParameterIndex::Tog_Reverse));

	traverser_resetter_.check(data.env_pitch, &block_traverser_);

	auto sample_pos = position_traverser_.get_positions(data.slider_speed->value, data.env_speed, block_traverser_, data.slider_sample_offset->value, kFloatsPerDSPVector);

	sample_pos /= float(buffer->song_rate) / buffer->sample_info->SR;

	SampleData sample_data(buffer->sample_info, buffer->channel_mode);

	const auto amp = plugin_->env_amp().search_vec(data.env_amp, block_positions, prev_pos) * data.slider_amp->value;

	if (data.toggle_reverse->value)
	{
		sample_pos = float(buffer->sample_info->num_frames - 1) - sample_pos;
	}

	// TODO: fudge implementation here!

	out_vec = add_noise(out_vec, data.option_noise_mode->index, data.env_noise_amount, data.env_noise_color, data.slider_noise_width, block_positions, prev_pos);
	out_vec = stereo_pan(out_vec, data.slider_pan->value, plugin_->env_pan(), data.env_pan, block_positions, prev_pos);
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
	const ml::DSPVector& block_positions,
	float prev_pos)
{
	ml::DSPVectorArray<2> out;

	if (env_noise_amount->points.count < 1)
	{
		return in;
	}

	const auto noise_amount = plugin_->env_noise_amount().search_vec(env_noise_amount, block_positions, prev_pos);

	if (ml::sum(noise_amount) < 0.0001f)
	{
		return in;
	}

	float noise_color;

	plugin_->env_noise_color().search_vec(env_noise_color, block_positions.getConstBuffer(), 1, prev_pos, &noise_color);

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

