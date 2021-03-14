#define BLKHDGEN_EXPORT
#include "plugin.h"

#include <blkhdgen/bind.hpp>
#include <blkhdgen/math.hpp>
#include <blkhdgen/standard_parameters.hpp>

#include "audio.h"

using namespace blkhdgen;

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

blkhdgen_UUID blkhdgen_get_plugin_uuid()
{
	return Classic::UUID;
}

blkhdgen_UUID blkhdgen_get_plugin_name()
{
	return Classic::NAME;
}

blkhdgen_Error blkhdgen_init()
{
	if (g_plugin) return blkhdgen_Error(Error::AlreadyInitialized);

	g_plugin = new Classic();

	return BLKHDGEN_OK;
}

blkhdgen_Error blkhdgen_terminate()
{
	if (!g_plugin) return blkhdgen_Error(Error::NotInitialized);

	delete g_plugin;

	return BLKHDGEN_OK;
}

blkhdgen_Sampler blkhdgen_make_sampler()
{
	if (!g_plugin) return { 0 };

	return bind::make_sampler<Audio>(g_plugin);
}

blkhdgen_Error blkhdgen_destroy_sampler(blkhdgen_Sampler sampler)
{
	return bind::destroy_sampler(sampler);
}

blkhdgen_Bool blkhdgen_sampler_requires_preprocessing()
{
	return Audio::REQUIRES_PREPROCESS ? BLKHDGEN_TRUE : BLKHDGEN_FALSE;
}

blkhdgen_Error blkhdgen_sampler_preprocess_sample(void* host, blkhdgen_PreprocessCallbacks callbacks, const blkhdgen_SampleInfo* sample_info)
{
	// TODO:
	return -1;
}

blkhdgen_Error blkhdgen_sampler_sample_deleted(blkhdgen_ID sample_id)
{
	// TODO:
	return -1;
}

int blkhdgen_get_num_groups()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_groups();
}

int blkhdgen_get_num_parameters()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_parameters();
}

blkhdgen_Group blkhdgen_get_group(blkhdgen_Index index)
{
	return bind::group(g_plugin->get_group(index));
}

blkhdgen_Group blkhdgen_get_group_by_id(blkhdgen_ID id)
{
	return bind::group(g_plugin->get_group_by_id(id));
}

blkhdgen_Parameter blkhdgen_get_parameter(blkhdgen_Index index)
{
	return bind::parameter(g_plugin->get_parameter(index));
}

blkhdgen_Parameter blkhdgen_get_parameter_by_uuid(blkhdgen_UUID uuid)
{
	return bind::parameter(g_plugin->get_parameter_by_uuid(uuid));
}

const char* blkhdgen_get_error_string(blkhdgen_Error error)
{
	switch (Error(error))
	{
		case Error::AlreadyInitialized: return "already initialized";
		case Error::NotInitialized: return "not initialized";
		default: return "unknown error";
	}
}

blkhdgen_Error blkhdgen_sampler_get_waveform_positions(const blkhdgen_SamplerBuffer* buffer, blkhdgen_FrameCount n, float* out, float* derivatives)
{
	if (!g_plugin) return blkhdgen_Error(Error::NotInitialized);

	g_plugin->gui().get_waveform_positions(g_plugin, buffer, n, out, derivatives);

	return BLKHDGEN_OK;
}