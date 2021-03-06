#define BLINK_EXPORT

#include "plugin.h"
#include "instance.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

namespace test {

Plugin::Plugin()
{
	auto spec_env_amp = std_params::envelopes::amp();
	auto spec_env_wave = std_params::envelopes::resonance();
	auto spec_env_p0 = std_params::envelopes::pitch();
	auto spec_env_p1 = std_params::envelopes::pitch();
	auto spec_env_fm0 = std_params::envelopes::resonance();
	auto spec_env_fm1 = std_params::envelopes::resonance();
	
	spec_env_amp.default_value = 0.5f;
	spec_env_wave.name = "wave";
	spec_env_p0.name = "p0";
	spec_env_p1.name = "p1";
	spec_env_fm0.name = "fm0";
	spec_env_fm1.name = "fm1";
	spec_env_wave.uuid = "337b8f6f-1d7a-41a7-ab05-802b23887b7a";
	spec_env_p0.uuid = "ce65f347-1891-413e-a3d4-177c6655d34c";
	spec_env_p1.uuid = "fc48a5e2-48a6-4021-8467-c196b215938d";
	spec_env_fm0.uuid = "05ffbc42-d2aa-4746-905e-44d4373b4345";
	spec_env_fm1.uuid = "343689ff-d1bf-48f0-96bf-7a6689568ef5";

	spec_env_amp.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_wave.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_p0.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_p1.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_fm0.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_fm1.flags |= blink_EnvelopeFlags_DefaultActive;

	env_amp_ = add_parameter(spec_env_amp);
	env_wave_ = add_parameter(spec_env_wave);
	env_p0_ = add_parameter(spec_env_p0);
	env_p1_ = add_parameter(spec_env_p1);
	env_fm0_ = add_parameter(spec_env_fm0);
	env_fm1_ = add_parameter(spec_env_fm1);

	auto group_noise = add_group("Noise");
	{
		auto spec_sld_noise_width = std_params::sliders::parameters::noise_width();

		spec_sld_noise_width.flags = blink_SliderFlags_NonGlobal;

		auto spec_env_noise_amount = std_params::envelopes::noise_amount();
		auto spec_env_noise_color = std_params::envelopes::noise_color();

		spec_env_noise_amount.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_color.sliders.push_back(blink_Index(ParameterIndex::Sld_NoiseWidth));
		spec_env_noise_amount.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));
		spec_env_noise_color.options.push_back(blink_Index(ParameterIndex::Option_NoiseMode));

		option_noise_mode_ = add_parameter(std_params::options::noise_mode());
		env_noise_amount_ = add_parameter(spec_env_noise_amount);
		env_noise_color_ = add_parameter(spec_env_noise_color);
		sld_noise_width_ = add_parameter(spec_sld_noise_width);

		env_noise_amount_->set_group_index(group_noise);
		env_noise_color_->set_group_index(group_noise);
	}
}

blink::SynthInstance* Plugin::make_instance()
{
	return new Instance(this);
}

Plugin* g_plugin = nullptr;

} // test

blink_UUID blink_get_plugin_uuid() { return test::Plugin::UUID; }
blink_UUID blink_get_plugin_name() { return test::Plugin::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (test::g_plugin) return blink_StdError_AlreadyInitialized;

	test::g_plugin = new test::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!test::g_plugin) return blink_StdError_NotInitialized;

	delete test::g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!test::g_plugin) return blink_StdError_NotInitialized;

	test::g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_SynthInstance blink_make_synth_instance()
{
	if (!test::g_plugin) return blink_SynthInstance{ 0 };

	return bind::synth_instance(test::g_plugin->add_instance());
}

blink_Error blink_destroy_synth_instance(blink_SynthInstance instance)
{
	if (!test::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (test::Instance*)(instance.proc_data);

	test::g_plugin->destroy_instance(obj);

	return BLINK_OK;
}

int blink_get_num_groups()
{
	if (!test::g_plugin) return 0;

	return test::g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!test::g_plugin) return 0;

	return test::g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(test::g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(test::g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(test::g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}