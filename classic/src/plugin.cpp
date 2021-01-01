#include <blkhdgen/bind.hpp>
#include <blkhdgen/math.hpp>
#include <blkhdgen/standard_parameters.hpp>
#include <blkhdgen/standard_traversers/classic.hpp>

#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

using namespace blkhdgen;

class Classic : public Generator
{
public:

	Classic()
	{
		amp_envelope_   = add_parameter(std_params::envelopes::amp());
		pan_envelope_   = add_parameter(std_params::envelopes::pan());
		pitch_envelope_ = add_parameter(std_params::envelopes::pitch());

		amp_slider_           = add_parameter(std_params::sliders::amp());
		pan_slider_           = add_parameter(std_params::sliders::pan());
		pitch_slider_         = add_parameter(std_params::sliders::pitch());
		sample_offset_slider_ = add_parameter(std_params::sliders::sample_offset());

		loop_toggle_    = add_parameter(std_params::toggles::loop());
		reverse_toggle_ = add_parameter(std_params::toggles::reverse());
	}

	const char* get_name() const override
	{
		return "Classic";
	}

	blkhdgen_Error process(blkhdgen_SR song_rate, blkhdgen_SR sample_rate, const blkhdgen_Position* pos, float** out) override
	{
		ml::DSPVectorArray<2> out_vec;
		
		// TODO: implement this

		stereo_pan(out_vec, pan_slider_->get(), pan_envelope_, pos);

		ml::store(out_vec.constRow(0), out[0]);
		ml::store(out_vec.constRow(1), out[1]);

		return BLKHDGEN_OK;
	}

	const char* get_error_string(blkhdgen_Error error) const override
	{
		return "Unknown error";
	}

	blkhdgen_Position get_waveform_position(blkhdgen_Position block_position, float* derivative = nullptr) const override
	{
		return waveform_traverser_(pitch_slider_->get(), pitch_envelope_->get_point_data(), block_position, sample_offset_slider_->get(), derivative);
	}

	void set_data_offset(int offset) override
	{
		pan_envelope_->set_data_offset(offset);
		traverser_.set_data_offset(offset);
		waveform_traverser_.set_data_offset(offset);
	}
	
private:

	mutable std_traversers::Classic traverser_;
	mutable std_traversers::Classic waveform_traverser_;

	std::shared_ptr<EnvelopeParameter> amp_envelope_;
	std::shared_ptr<EnvelopeParameter> pan_envelope_;
	std::shared_ptr<EnvelopeParameter> pitch_envelope_;
	std::shared_ptr<SliderParameter<float>> amp_slider_;
	std::shared_ptr<SliderParameter<float>> pan_slider_;
	std::shared_ptr<SliderParameter<float>> pitch_slider_;
	std::shared_ptr<SliderParameter<int>> sample_offset_slider_;
	std::shared_ptr<ToggleParameter> loop_toggle_;
	std::shared_ptr<ToggleParameter> reverse_toggle_;
};

static ml::DSPVectorArray<2> stereo_pan(const ml::DSPVectorArray<2> in, float pan, std::shared_ptr<EnvelopeParameter> pan_envelope, const blkhdgen_Position* pos)
{
	auto out = in;

	ml::DSPVector env_pan;

	for (int i = 0; i < kFloatsPerDSPVector; i++)
	{
		env_pan[i] = pan_envelope->get_mod_value(pos[i]);
	}

	const auto zero = ml::DSPVector(0.0f);
	const auto one = ml::DSPVector(1.0f);

	env_pan = ml::clamp(env_pan + pan, ml::DSPVector(-1.0f), ml::DSPVector(1.0f));

	const auto pan_amp_L = ml::lerp(one, zero, ml::max(zero, env_pan));
	const auto pan_amp_R = ml::lerp(one, zero, ml::max(zero, 0.0f - env_pan));

	const auto pan_vec = ml::concatRows(pan_amp_L, pan_amp_R);

	out *= pan_vec;

	out.row(0) += out.row(1) * (1.0f - pan_amp_R);
	out.row(1) += out.row(0) * (1.0f - pan_amp_L);

	return out;
}

blkhdgen_Generator make_generator()
{
	return bind::make_generator<Classic>();
}

blkhdgen_Error destroy_generator(blkhdgen_Generator generator)
{
	return bind::destroy_generator(generator);
}