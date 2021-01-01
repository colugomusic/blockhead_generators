#include <blkhdgen/bind.hpp>
#include <blkhdgen/math.hpp>
#include <blkhdgen/standard_parameters.hpp>
#include <blkhdgen/standard_traversers/classic.hpp>

using namespace blkhdgen;

class Classic : public Generator
{
public:

	Classic()
	{
		add_parameter(std_params::envelopes::amp());
		add_parameter(std_params::envelopes::pan());
		pitch_envelope_ = add_parameter(std_params::envelopes::pitch());

		add_parameter(std_params::sliders::amp());
		add_parameter(std_params::sliders::pan());
		pitch_slider_ = add_parameter(std_params::sliders::pitch());
		sample_offset_slider_ = add_parameter(std_params::sliders::sample_offset());

		add_parameter(std_params::toggles::loop());
		add_parameter(std_params::toggles::reverse());

		OptionSpec test_option;

		test_option.uuid = "";
		test_option.name = "Test Option";
		test_option.group_id = 0;
		test_option.flags = 0;
		test_option.default_value = 0;

		test_option.options[0] = "Option A";
		test_option.options[1] = "Option B";
		test_option.options[2] = "Option C";

		add_parameter(test_option);
	}

	const char* get_name() const override
	{
		return "Classic";
	}

	blkhdgen_Error process(blkhdgen_SR song_rate, blkhdgen_SR sample_rate, const blkhdgen_Position* pos, float** out) override
	{
		// TODO: implement
		return BLKHDGEN_OK;
	}

	const char* get_error_string(blkhdgen_Error error) const override
	{
		return "Unknown error";
	}

	blkhdgen_Position get_waveform_position(blkhdgen_Position block_position) const override
	{
		return waveform_traverser_(pitch_slider_->get(), pitch_envelope_->get_point_data(), block_position, sample_offset_slider_->get());
	}
	
private:

	mutable std_traversers::Classic traverser_;
	mutable std_traversers::Classic waveform_traverser_;

	std::shared_ptr<EnvelopeParameter> pitch_envelope_;
	std::shared_ptr<SliderParameter<float>> pitch_slider_;
	std::shared_ptr<SliderParameter<int>> sample_offset_slider_;
};

blkhdgen_Generator make_generator()
{
	return bind::make_generator<Classic>();
}

blkhdgen_Error destroy_generator(blkhdgen_Generator generator)
{
	return bind::destroy_generator(generator);
}