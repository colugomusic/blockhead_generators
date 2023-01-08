#define BLINK_EXPORT
#include "plugin.h"
#include <blink/bind_effect.hpp>
#include <blink/errors.hpp>

namespace ring_modulator { Plugin* g_plugin {}; }

using namespace blink;
using namespace ring_modulator;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out;

	out.uuid = "881569c5-108b-4923-b30e-1e1ac6c1a04f";
	out.name = "Ring Modulator";
	out.category = BLINK_STD_CATEGORY_DESTRUCTION;
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

	g_plugin = new ring_modulator::Plugin();

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

CMRC_DECLARE(ring_modulator);

blink_ResourceData blink_get_resource_data(const char* path)
{
	return g_plugin->get_resource_data(cmrc::ring_modulator::get_filesystem(), path);
}