#define BLINK_EXPORT
#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

SpringReverb::SpringReverb()
{
	// TODO: do this properly
	auto spec_env_size = std_params::envelopes::resonance();
	auto spec_env_decay = std_params::envelopes::resonance();
	auto spec_env_mix = std_params::envelopes::mix();

	spec_env_size.name = "Size";
	spec_env_size.uuid = "65e00902-5318-4d76-9b86-2479dcce7f52";
	spec_env_decay.name = "Decay";
	spec_env_decay.uuid = "aa47aa82-0e26-4d3b-8f60-ddb5d57353e7";
	spec_env_size.default_value = 0.5f;
	spec_env_decay.default_value = 0.5f;
	spec_env_mix.default_value = 0.25f;

	spec_env_size.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_decay.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_mix.flags |= blink_EnvelopeFlags_DefaultActive;

	env_size_ = add_parameter(spec_env_size);
	env_decay_ = add_parameter(spec_env_decay);
	env_mix_ = add_parameter(spec_env_mix);
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

SpringReverb* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid() { return SpringReverb::UUID; }
blink_UUID blink_get_plugin_name() { return SpringReverb::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new SpringReverb();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Effect blink_make_effect(int instance_group)
{
	if (!g_plugin) return blink_Effect { 0 };

	return bind::make_effect<Audio>(g_plugin);
}

blink_Error blink_destroy_effect(blink_Effect effect)
{
	return bind::destroy_effect(effect);
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