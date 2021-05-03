#define BLINK_EXPORT
#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include <blink/standard_parameters.hpp>

using namespace blink;

Filter::Filter()
{
	auto spec_env_freq = std_params::envelopes::filter_frequency();
	auto spec_env_res = std_params::envelopes::resonance();
	auto spec_env_mix = std_params::envelopes::mix();

	spec_env_freq.flags |= blink_EnvelopeFlags_DefaultActive;
	spec_env_res.flags |= blink_EnvelopeFlags_DefaultActive;

	env_freq_ = add_parameter(spec_env_freq);
	env_res_ = add_parameter(spec_env_res);
	env_mix_ = add_parameter(spec_env_mix);
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Filter* g_plugin = nullptr;



blink_UUID blink_get_plugin_uuid() { return Filter::UUID; }
blink_UUID blink_get_plugin_name() { return Filter::NAME; }
const char* blink_get_plugin_category() { return BLINK_STD_CATEGORY_FILTERS; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Filter();

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
	if (!g_plugin) return blink_Effect { 0, 0, 0 };

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