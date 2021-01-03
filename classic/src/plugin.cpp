#define BLKHDGEN_EXPORT

#include <blkhdgen/bind.hpp>
#include <blkhdgen/math.hpp>
#include <blkhdgen/standard_parameters.hpp>
#include <blkhdgen/standard_traversers/classic.hpp>

using namespace blkhdgen;

class Classic : public Generator
{
public:

	static constexpr auto UUID = "bd64e4c8-f788-433b-a42a-d375afd92503";
	static constexpr auto NAME = "Classic";
	static constexpr auto REQUIRES_PREPROCESS = false;

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

	blkhdgen_Error process(blkhdgen_SR song_rate, blkhdgen_SR sample_rate, const blkhdgen_Position* block_pos, float** out) override
	{
		ml::DSPVectorArray<2> out_vec;
		ml::DSPVector env_amp;

		const auto data_offset = get_data_offset();

		audio_block_traverser_.generate(block_pos, data_offset);

		const auto sample_info = get_sample_info();

		auto sample_pos = audio_position_traverser_.get_positions(pitch_slider_->get(), *pitch_envelope_.get(), &audio_block_traverser_, sample_offset_slider_->get());

		sample_pos /= float(song_rate) / sample_info->SR;

		env_amp = amp_envelope_->get_mod_values(&audio_block_traverser_);

		if (reverse_toggle_.get())
		{
			sample_pos = float(sample_info->num_frames - 1) - sample_pos;
		}

		if (sample_info->num_channels > 0)
		{
			out_vec.row(0) = read_sample_frames_interp(0, sample_pos, loop_toggle_.get());
		}

		if (sample_info->num_channels > 1)
		{
			out_vec.row(1) = read_sample_frames_interp(1, sample_pos, loop_toggle_.get());
		}
		else
		{
			out_vec.row(1) = out_vec.row(0);
		}

		stereo_pan(out_vec, pan_slider_->get(), pan_envelope_, &audio_block_traverser_);

		out_vec *= ml::repeatRows<2>(env_amp);

		ml::storeAligned(out_vec.constRow(0), out[0]);
		ml::storeAligned(out_vec.constRow(1), out[1]);

		return BLKHDGEN_OK;
	}

	const char* get_error_string(blkhdgen_Error error) const override
	{
		return "Unknown error";
	}

	blkhdgen_Error get_waveform_positions(const blkhdgen_Position* block_positions, float* out, float* derivatives = nullptr) const override
	{
		gui_block_traverser_.generate(block_positions, get_data_offset());

		const auto positions = gui_position_traverser_.get_positions(pitch_slider_->get(), *pitch_envelope_.get(), &gui_block_traverser_, sample_offset_slider_->get(), derivatives);

		ml::storeAligned(positions, out);

		return BLKHDGEN_OK; 
	}

	size_t get_required_aux_buffer_size() const override
	{
		return 0;
	}

	blkhdgen_Error preprocess_sample(void* host, blkhdgen_PreprocessCallbacks callbacks) const
	{
		return BLKHDGEN_OK;
	}

private:

	Traverser audio_block_traverser_;
	mutable Traverser gui_block_traverser_;

	mutable std_traversers::Classic audio_position_traverser_;
	mutable std_traversers::Classic gui_position_traverser_;

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

blkhdgen_Generator make_generator()
{
	return bind::make_generator<Classic>();
}

blkhdgen_Error destroy_generator(blkhdgen_Generator generator)
{
	return bind::destroy_generator(generator);
}