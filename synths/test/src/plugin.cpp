#define BLINK_EXPORT
#include "plugin.h"
#include <blink/bind_synth.hpp>
#include <blink/errors.hpp>

namespace test { Plugin* g_plugin {}; }

using namespace blink;
using namespace test;

blink_PluginInfo blink_get_plugin_info()
{
	blink_PluginInfo out = blink_PluginInfo();

	out.uuid = "bb0c455a-dae3-498d-b929-f18a20764528";
	out.name = "test synth";
	out.version = PLUGIN_VERSION;
	out.has_icon = true;

	return out;
}

blink_Error blink_init()
{
	if (g_plugin) return blink_StdError_AlreadyInitialized;

	g_plugin = new test::Plugin();

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
	if (!g_plugin) return blink_SynthInstance{ 0 };

	return bind::synth_instance(g_plugin->add_instance());
}

blink_Error blink_destroy_synth_instance(blink_SynthInstance instance)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	return g_plugin->destroy_instance(std::move(instance));
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

blink_Error blink_get_envelope_manipulator_target(blink_UUID uuid, blink_EnvelopeManipulatorTarget* out)
{
	if (!g_plugin) return blink_StdError_NotInitialized;

	return g_plugin->get_envelope_manipulator_target(uuid, out);
}

const char* blink_get_error_string(blink_Error error)
{
	return blink::get_std_error_string(blink_StdError(error));
}

CMRC_DECLARE(test);

blink_ResourceData blink_get_resource_data(const char* path)
{
	return g_plugin->get_resource_data(cmrc::test::get_filesystem(), path);
}