#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

#include "audio.h"

using namespace blink;

Classic::Classic()
{
	env_amp_ = add_parameter(std_params::envelopes::amp());
	env_pan_ = add_parameter(std_params::envelopes::pan());
	env_pitch_ = add_parameter(std_params::envelopes::pitch());

	sld_amp_ = add_parameter(std_params::sliders::amp());
	sld_pan_ = add_parameter(std_params::sliders::pan());
	sld_pitch_ = add_parameter(std_params::sliders::pitch());
	sld_sample_offset_ = add_parameter(std_params::sliders::sample_offset());

	tog_loop_ = add_parameter(std_params::toggles::loop());
	tog_revers_ = add_parameter(std_params::toggles::reverse());
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

blink_Group blink_get_group_by_id(blink_ID id)
{
	return bind::group(g_plugin->get_group_by_id(id));
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