#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind.hpp>
#include <blink/errors.hpp>

namespace lofi { Plugin* g_plugin = nullptr; }

using namespace blink;
using namespace lofi;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "90e5db1b-19c0-4ad5-aecf-3622db865584";
	out.name = "Lofi";
	out.category = BLINK_STD_CATEGORY_DESTRUCTION;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

	g_plugin = new lofi::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	delete g_plugin;

	return BLINK_OK;
}

blink_Error blink_stream_init(blink_SR SR)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	g_plugin->stream_init(SR);

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!g_plugin) return blink_EffectInstance{0};

	return bind::effect_instance(g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	const auto obj = (lofi::Instance*)(instance.proc_data);

	g_plugin->destroy_instance(obj);

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
	return bind::parameter(g_plugin->get_parameter_by_uuid(uuid));
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

CMRC_DECLARE(lofi);

blink_ResourceData blink_get_resource_data(const char* path)
{
	if (g_plugin->resources().has(path)) return g_plugin->resources().get(path);

	const auto fs = cmrc::lofi::get_filesystem();

	if (!fs.exists(path)) return { 0, 0 };
	if (!fs.is_file(path)) return { 0, 0 };

	const auto file = fs.open(path);

	return g_plugin->resources().store(path, file);
}