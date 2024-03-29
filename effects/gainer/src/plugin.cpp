#define BLINK_EXPORT
#include "plugin.h"

#include <blink/bind_effect.hpp>
#include <blink/errors.hpp>

namespace gainer { Plugin* g_plugin {}; }

using namespace blink;
using namespace gainer;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "3215908f-fe70-4f0b-b156-90288c2ee293";
	out.name = "Gainer";
	out.category = BLINK_STD_CATEGORY_UTILITY;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

	g_plugin = new gainer::Plugin();

	return BLINK_OK;
}

blink_Error blink_terminate()
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	delete g_plugin;

	return BLINK_OK;
}

blink_EffectInstance blink_make_effect_instance()
{
	if (!g_plugin) return blink_EffectInstance{ 0 };

	return bind::effect_instance(g_plugin->add_instance());
}

blink_Error blink_destroy_effect_instance(blink_EffectInstance instance)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	return g_plugin->destroy_instance(std::move(instance));
}

int blink_get_num_parameters()
{
	if (!g_plugin) return 0;

	return g_plugin->get_num_parameters();
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

CMRC_DECLARE(gainer);

blink_ResourceData blink_get_resource_data(const char* path)
{
	return g_plugin->get_resource_data(cmrc::gainer::get_filesystem(), path);
}