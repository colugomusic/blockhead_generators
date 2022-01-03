#define BLINK_EXPORT

#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>
#include "parameters.h"

using namespace blink;

namespace berk { Plugin* g_plugin = nullptr; }

using namespace berk;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "e7804e4e-fdf6-4545-aa4f-77fed08d892a";
	out.name = "Berk";
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

	g_plugin = new berk::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	delete g_plugin;

	return BLINK_OK;
}

blink_SynthInstance blink_make_synth_instance()
{
	if (!berk::g_plugin) return blink_SynthInstance{ 0 };

	return bind::synth_instance(berk::g_plugin->add_instance());
}

blink_Error blink_destroy_synth_instance(blink_SynthInstance instance)
{
	if (!berk::g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (berk::Instance*)(instance.proc_data);

	berk::g_plugin->destroy_instance(obj);

	return BLINK_OK;
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
	return bind::parameter(g_plugin->get_parameter(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

CMRC_DECLARE(berk);

blink_ResourceData blink_get_resource_data(const char* path)
{
	if (g_plugin->resources().has(path)) return g_plugin->resources().get(path);

	const auto fs = cmrc::berk::get_filesystem();

	if (!fs.exists(path)) return { 0, 0 };
	if (!fs.is_file(path)) return { 0, 0 };

	const auto file = fs.open(path);

	return g_plugin->resources().store(path, file);
}