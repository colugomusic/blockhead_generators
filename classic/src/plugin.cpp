#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

#include "audio.h"

using namespace blink;

Classic::Classic()
{
	auto spec_env_amp = std_params::envelopes::amp();
	auto spec_env_pan = std_params::envelopes::pan();
	auto spec_env_pitch = std_params::envelopes::pitch();

	spec_env_amp.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_pan.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_pitch.flags |= blink_EnvelopeFlags_DefaultActive;

	env_amp_ = add_parameter(spec_env_amp);
	env_pan_ = add_parameter(spec_env_pan);
	env_pitch_ = add_parameter(spec_env_pitch);

	sld_amp_ = add_parameter(std_params::sliders::amp());
	sld_pan_ = add_parameter(std_params::sliders::pan());
	sld_pitch_ = add_parameter(std_params::sliders::pitch());
	sld_sample_offset_ = add_parameter(std_params::sliders::sample_offset());

	tog_loop_ = add_parameter(std_params::toggles::loop());
	tog_revers_ = add_parameter(std_params::toggles::reverse());

	auto test_group = add_group("Test Group");

	blink::EnvelopeSpec test_env;

	test_env.uuid = "40d536fe-8305-4d84-a671-0d7899280db2";
	test_env.name = "Test Envelope";
	test_env.display_value = std_params::display_number;
	test_env.from_string = std_params::find_number<float>;
	test_env.group_index = test_group;

	add_parameter(test_env);

	test_env.uuid = "e171f2c5-b62c-4887-8075-63f920bb46ea";
	test_env.name = "Another parameter";
	test_env.display_value = std_params::display_number;
	test_env.from_string = std_params::find_number<float>;
	test_env.group_index = test_group;

	add_parameter(test_env);
}

GUI& Classic::gui()
{
	return gui_;
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Classic* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid()
{
	return Classic::UUID;
}

blink_UUID blink_get_plugin_name()
{
	return Classic::NAME;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Classic();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Sampler blink_make_sampler()
{
	if (!g_plugin) return { 0 };

	return bind::make_sampler<Audio>(g_plugin);
}

blink_Error blink_destroy_sampler(blink_Sampler sampler)
{
	return bind::destroy_sampler(sampler);
}

blink_Bool blink_sampler_requires_preprocessing()
{
	return Audio::REQUIRES_PREPROCESS ? BLINK_TRUE : BLINK_FALSE;
}

blink_Error blink_sampler_preprocess_sample(void* host, blink_PreprocessCallbacks callbacks, const blink_SampleInfo* sample_info)
{
	// TODO:
	return -1;
}

blink_Error blink_sampler_sample_deleted(blink_ID sample_id)
{
	// TODO:
	return -1;
}

int blink_get_num_groups()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_groups();
}

int blink_get_num_parameters()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_parameters();
}

blink_Group blink_get_group(blink_Index index)
{
	return bind::group(g_plugin->get_group(index));
}

blink_Parameter blink_get_parameter(blink_Index index)
{
	return bind::parameter(g_plugin->get_parameter(index));
}

blink_Parameter blink_get_parameter_by_uuid(blink_UUID uuid)
{
	return bind::parameter(g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	switch (Error(error))
	{
		case Error::AlreadyInitialized: return "already initialized";
		case Error::NotInitialized: return "not initialized";
		default: return "unknown error";
	}
}

blink_Error blink_sampler_get_waveform_positions(const blink_SamplerBuffer* buffer, blink_FrameCount n, float* out, float* derivatives, float* amp)
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	g_plugin->gui().get_waveform_positions(g_plugin, buffer, n, out, derivatives, amp);

	return BLINK_OK;
}