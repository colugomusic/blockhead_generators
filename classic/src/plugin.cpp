#include <blkhdgen/bind.hpp>
#include <blkhdgen/math.hpp>
#include <blkhdgen/standard_parameters.hpp>

using namespace blkhdgen;

class Classic : public Generator
{
public:

	Classic()
	{
		add_parameter(std_params::envelopes::amp());
		add_parameter(std_params::envelopes::pan());
		add_parameter(std_params::envelopes::pitch());

		add_parameter(std_params::sliders::amp());
		add_parameter(std_params::sliders::pan());
		add_parameter(std_params::sliders::pitch());

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

	blkhdgen_Error process(const blkhdgen_Position* pos, float** out) override
	{
		// TODO: implement
		return BLKHDGEN_OK;
	}

	const char* get_error_string(blkhdgen_Error error) const override
	{
		return "Unknown error";
	}
};


blkhdgen_Generator make_generator()
{
	return bind::make_generator<Classic>();
}

blkhdgen_Error destroy_generator(blkhdgen_Generator generator)
{
	return bind::destroy_generator(generator);
}