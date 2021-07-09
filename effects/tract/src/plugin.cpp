#define BLINK_EXPORT
#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>

using namespace blink;

namespace tract {

Tract::Tract()
	: params_(this)
{
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

} // tract

tract::Tract* g_plugin = nullptr;

blink_UUID blink_get_plugin_uuid() { return tract::Tract::UUID; }
blink_UUID blink_get_plugin_name() { return tract::Tract::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(tract::Error::AlreadyInitialized);

	g_plugin = new tract::Tract();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(tract::Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!g_plugin) return blink_Error(tract::Error::NotInitialized);

	g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_Effect blink_make_effect(int instance_group)
{
	if (!g_plugin) return blink_Effect{ 0, 0 };

	const auto instance = new tract::Audio(g_plugin, instance_group);
	const auto out = bind::effect(instance);

	g_plugin->register_instance(instance);

	return out;
}

blink_Error blink_destroy_effect(blink_Effect effect)
{
	if (!g_plugin) return blink_Error(tract::Error::NotInitialized);

	g_plugin->unregister_instance((blink::Effect*)(effect.proc_data));

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
	switch (tract::Error(error))
	{
		case tract::Error::AlreadyInitialized: return "already initialized";
		case tract::Error::NotInitialized: return "not initialized";
		default: return "unknown error";
	}
}