#define BLINK_EXPORT
#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

Test::Test()
{
	auto spec_env_wave = std_params::envelopes::resonance();
	auto spec_env_p0 = std_params::envelopes::pitch();
	auto spec_env_p1 = std_params::envelopes::pitch();
	auto spec_env_fm0 = std_params::envelopes::resonance();
	auto spec_env_fm1 = std_params::envelopes::resonance();

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

	spec_env_wave.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_p0.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_p1.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_fm0.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_fm1.flags |= blink_EnvelopeFlags_DefaultActive;

	env_wave_ = add_parameter(spec_env_wave);
	env_p0_ = add_parameter(spec_env_p0);
	env_p1_ = add_parameter(spec_env_p1);
	env_fm0_ = add_parameter(spec_env_fm0);
	env_fm1_ = add_parameter(spec_env_fm1);
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Test* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid()
{
	return Test::UUID;
}

blink_UUID blink_get_plugin_name()
{
	return Test::NAME;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Test();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Synth blink_make_synth()
{
	if (!g_plugin) return blink_Synth { 0 };

	return bind::make_synth<Audio>(g_plugin);
}

blink_Error blink_destroy_synth(blink_Synth synth)
{
	return bind::destroy_synth(synth);
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