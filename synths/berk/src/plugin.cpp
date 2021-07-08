#define BLINK_EXPORT

#include "plugin.h"
#include "audio.h"

#include <blink/bind.hpp>
#include <blink/math.hpp>
#include "parameters.h"

using namespace blink;

namespace berk {

Berk::Berk()
	: params_(this)
{
}

enum class Error
{
	AlreadyInitialized,
	NotInitialized,
};

Berk* g_plugin = nullptr;

} // berk

using namespace berk;

blink_UUID blink_get_plugin_uuid() { return Berk::UUID; }
blink_UUID blink_get_plugin_name() { return Berk::NAME; }
const char* blink_get_plugin_version() { return PLUGIN_VERSION; }

blink_Error blink_init()
{
	if (g_plugin) return blink_Error(Error::AlreadyInitialized);

	g_plugin = new Berk();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_Error(Error::NotInitialized);

	delete g_plugin;

	return BLINK_OK;
}

blink_Synth blink_make_synth(int instance_group)
{
	if (!g_plugin) return blink_Synth { 0, 0 };

	return bind::make_synth<Audio>(g_plugin, instance_group);
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